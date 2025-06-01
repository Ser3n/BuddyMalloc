//////////////////////////////////////////////////////////////////////////////////
//
//   Program Name:  Buddy System Algorithm
//
//   Description:  Buddy System Algorithm
//
//   Student name:
//
//
//
//////////////////////////////////////////////////////////////////////////////////

////////////////////PSEUDO CODE/////////////////////////////////////////
// Buddy_malloc(int memory_request_of_n_bytes)
///////////////////////////////////////////////////////////////////////////////////////
// INPUT: memory_request_of_n_bytes
// OUTPUT: pointer to the block allocated (void*)
// ///////////////////////////////////////////////////////////////////////////////////////
//    1) As memory blocks are represented as Nodes in a linked list, we need to take into account
//        the amount of memory consumed by a Node structure.
//        Given a memory request of n bytes, the Node that can satisfy this request must include the sizeof(Node).
//        Therefore, incorporate the sizeof(Node) as part of the memory request.
//    memory_request = n bytes + sizeof(Node);  // make sure the data fits within a Node
//    2) Find the smallest bin (k) that could accommodate the memory request.
//    We want to find the appropriate value for index k, in the expression 2^k (the size of the block).  This is our desired bin.
//    *3)Check if the desired bin (k) found is greater than the MAXIMUM possible bin size,
//           if yes, then there's not enough memory to satisfy the request. return NULL.
//    4)If (there's a free block in the FreeList whose index is equal to the desiredBin (k) ),
//         then allocate that block to satisfy the request.  This is the block perfectly matching the request.
//                 This also means that you need to remove that particular block from the FreeList -> that block is now allocated.
//                 set the properties for that block (e.g. size)
//                 return the base address of the data section of the block found to satisfy  the request.
//                 This data address is equal to the base address of the free block + sizeof(Node)
//    5) Else if (the desired block size is not available), then find an alternative bin size (the next larger bin  available):
//       ...
//       remove a free block from the alternative bin.
//       LOOP: (objective: Find the MINIMAL 2^k block that is larger or equal to that of the requested memory)
//          Split the free block removed.
//          Add the smaller blocks to the appropriate bin in the Free List (this is the new alternative bin). Make sure that the connections in the linked list are handled correctly.
//          Pick one of the smaller blocks from the new alternative bin (remove this block from the Free List).
//          Execute statements in the loop until the block removed is the smallest bin that could satisfy the memory request.
//       set the properties for the block (e.g. size) found to satisfy the request.
//       return the base address of the data section of the block found to satisfy the request.
//       This data address is equal to the base address of the free block + sizeof(Node)
//       ...

#include "buddysys.h"

bool DEBUG = false; // Set to true to enable debug mode

/////////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////////
Node *FreeList[MAX_POSSIBLE_ROWS]; // Static array of Node pointers
int Max_Rows = 0;                  // Actual number of rows used

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
void printNode(Node *node, int k)
{
    if (node == NULL)
    {
        cout << "Node is NULL" << endl;
        return;
    }
    cout << "===========Node Details:===========" << endl;
    cout << "Node Address: " << node << endl;
    cout << "Size: " << node->size << " bytes" << endl;
    cout << "Actual Size: " << node->size + sizeof(Node) << " bytes" << endl;   // Actual size including Node header
    cout << "Block Size: " << powerOfTwo(k + MIN_ROW_SIZE) << " bytes" << endl; // Size of the block in bytes
    cout << "Allocated: " << (node->alloc == 1 ? "TRUE" : "FALSE") << endl;
    cout << "Next: " << node->next << endl;
    cout << "Previous: " << node->previous << endl;
    cout << "===================================" << endl;
}
// This function removes the head of the FreeList for the bin k
void headRemoval(int k)
{
    if (FreeList[k] == NULL) // Check if it is empty
    {
        return;
    }

    Node *block = FreeList[k];

    FreeList[k] = block->next; // Move the head to the next block in the list

    if (FreeList[k] != NULL) // Ciheck if it is NULL, so we dont get a segmentation fault
    {
        FreeList[k]->previous = NULL; // Set the previous pointer of the new head to NULL
    }

    // Clean up removed block's pointers
    block->next = NULL;
    block->previous = NULL;

    if (DEBUG)
    {
        cout << "Removed block of size " << block->size << " from FreeList[" << k << "]" << endl;
    }
}
// 2^k
int powerOfTwo(int k)
{
    return 1 << k; // 2^k
}

int smallestK(int memory_request_max)
{
    int k = 0;
    int size = MIN_BLOCK_SIZE;
    while (size < memory_request_max)
    {
        size *= 2; // size^2
        k++;
    }
    if (DEBUG)
    {
        cout << "Smallest k for memory request of " << memory_request_max << " is: " << k << endl;
        cout << "Size of block: " << size << " bytes" << endl;
    }

    int check = powerOfTwo(k + MIN_ROW_SIZE); // Calculate the size of the block for the smallest k
    int check2 = 1;
    int j = 0;

    while (check2 < memory_request_max)
    {
        check2 *= 2;
        j++;
        // if (DEBUG)
        // {
        //     cout << "Check2: " << check2 << " bytes" << endl;
        //     cout << "Check2 iteration: " << j << endl;
        // }
    }

    if (DEBUG)
    {
        cout << "Power of two for k = " << k << " is: " << check << " bytes" << endl;
        cout << "k = " << k << " Check = " << check2 << " bytes" << endl;
    }
    if (size == check2 || size == check) // Check if the size matches the power of two
    {
        if(DEBUG)
        {
            cout << "Size checks out!" << endl;
        }
    }
    else
    {
        cout << "Size does not check out" << endl;
        return -1; // If the size does not match the power of two, return -1
    }

    return k; // Return the smallest bin that can accommodate the request
}

void insertToFreeList(Node *block, int k)
{
    if (DEBUG)
    {
        cout << "Inserting block into FreeList[" << k << "]" << endl;
        printNode(block, k); // Print the details of the block to be inserted
    }

    block->next = FreeList[k]; // Insert the block at the head of the FreeList[k]
    block->previous = NULL;    // Set the previous pointer to NULL as it will be the new head
    if (FreeList[k] != NULL)   // Check if there is a block in the FreeList[k]
    {
        FreeList[k]->previous = block; // Set the previous pointer of the head of the list to point to the new block
    }
    FreeList[k] = block; // Make the new block the head of the FreeList[k]

    if (DEBUG)
    {
        cout << "Block inserted into FreeList[" << k << "]" << endl;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Buddy functions
/////////////////////////////////////////////////////////////////////////////////////////////////////

void *buddyMalloc(int user_memory_request)
{
    if (DEBUG)
    {
        cout << "=================Buddy Malloc====================\n";
        cout << "Requested memory size: " << user_memory_request << " bytes" << endl;
    }
    // 1)incorporate the sizeof(Node) as part of the memory request
    long long int memory_request_max = user_memory_request + sizeof(Node); // Include the size of the Node header
    // Find the Maximum Rows based on MEMORYSIZE and MIN_BLOCK_SIZE
    Max_Rows = getRows(MEMORYSIZE, MIN_BLOCK_SIZE);

    if (DEBUG)
    {
        cout << "Memory request including Node header: " << memory_request_max << " bytes" << endl;
        cout << "Max Rows: " << Max_Rows << endl;
    }

    if (memory_request_max > MEMORYSIZE)
    {
        cout << "Error: Requested memory exceeds maximum block size." << endl;
        return NULL; // Not enough memory to satisfy the request
    }

    // 2) Find the smallest bin (k) that could accommodate the memory request.
    int k = smallestK(memory_request_max);

    if (DEBUG)
    {
        if (k == -1)
        {
            cout << "Error: No suitable bin found for the requested memory size." << endl;
            return NULL; // No suitable bin found
        }
        // 3)Check if the desired bin (k) found is greater than the MAXIMUM possible size k,
        else if (k >= Max_Rows)
        {
            cout << "Error: The requested bin size exceeds the maximum block size." << endl;
            return NULL; // Requested bin size exceeds maximum block size
        }
    }

    cout << "Allocating memory from bin: " << k << endl;

    int orig_k = k;
    // lets find an available block in the FreeList, start from desired size k and increment until a suitable block is found
    while (k < Max_Rows && FreeList[k] == NULL)
    {
        if (DEBUG)
            cout << "FreeList[" << k << "] is empty, checking next size k = " << (k + 1) << endl;
        k++;
    }

    if (k >= Max_Rows)
    {
        cout << "Error: No suitable block found in FreeList." << endl;
        return NULL; // No suitable block found
    }

    // Found a suitable block in the FreeList
    if (DEBUG)
    {
        cout << "Found available block in FreeList[" << k << "]" << endl;
    }

    Node *allocated_block = FreeList[k];
    if (DEBUG)
    {
        cout << "Allocated block address: " << allocated_block << endl;
        cout << "Details: " << endl;
        printNode(allocated_block, k);
    }
    headRemoval(k); // Remove the head of the FreeList for the bin k

    if (DEBUG)
    {
        cout << "Removed block from FreeList[" << k << "]" << endl;
        cout << "Allocated block Details: " << endl;
        printNode(allocated_block, k); // Print the details of the allocated block
    }

    while (k > orig_k) // If the block is larger than the requested size, we need to split it, Else it satisfies the request and no splitting needed
    {

        if (DEBUG)
        {
            cout << "Splitting block of size " << powerOfTwo(k + MIN_ROW_SIZE)
                 << " into two blocks of size " << powerOfTwo(k + MIN_ROW_SIZE - 1) << endl;
        }
        // Split the block into two halves
        int block_size = powerOfTwo(k + MIN_ROW_SIZE); // Current block size in bytes
        int half_size = block_size / 2;                // Half size

        // Second half starts at first_half + half_size
        Node *second_half = (Node *)((byte *)allocated_block + half_size);
        if (DEBUG)
        {
            cout << "Second half starts at address: " << second_half << endl;
            printNode(second_half, k); // Print the details of the second half
        }
        if (DEBUG)
        {
            cout << "Setting up second half block at address: " << second_half << endl;
        }

        // initialize the second half
        second_half->size = half_size - sizeof(Node); // Data size only
        second_half->alloc = 0;                       // Free
        second_half->next = NULL;
        second_half->previous = NULL;

        if (DEBUG)
        {
            cout << "Second half block Details: " << endl;
            printNode(second_half, k); // Print the details of the second half
        }

        // Add second half to free list
        k--; // Move to smaller size class

        second_half->next = FreeList[k]; // Tjis is pointing to the head of the list FreeList[k]

        if (FreeList[k] != NULL) // Check if there is a block in the FreeList[k]
        {
            if (DEBUG)
            {
                cout << "Linking second half to FreeList[" << k << "]" << endl;
            }
            // Set the previous pointer of the head of the list to point to the second half
            FreeList[k]->previous = second_half;
        }
        else
        {
            if (DEBUG)
            {
                cout << "FreeList[" << k << "] is empty, setting second half as head." << endl;
            }
        }
        // Make the second half the new head of FreeList[k]
        FreeList[k] = second_half;

        if (DEBUG)
        {
            cout << "Added second half to FreeList[" << k << "]" << endl;
        }
    }

    // Set up allocated block
    allocated_block->size = user_memory_request; // Store user data size only
    allocated_block->alloc = 1;                  // Block has been allocated
    allocated_block->next = NULL;                // Not pointing to anything as it has been removed from the FreeList
    allocated_block->previous = NULL;            // Not pointing to anything as it has been removed from the FreeList

    if (DEBUG)
    {
        cout << "Allocated block Details: " << endl;
        printNode(allocated_block, k); // Print the details of the allocated block
    }
    if (DEBUG)
    {
        cout << "Allocated block at address: " << allocated_block << endl;
    }

    if (DEBUG)
        cout << "=== buddyMalloc completed ===" << endl;
    // TODO return pointers
    return (void *)(allocated_block + 1); // Return pointer to user data
}

int buddyFree(void *p)
{
    // STEP 1 : INITIALIZATION
    if (DEBUG)
    {
        cout << "=================Buddy Free====================\n";
        cout << "Pointer to user data: " << p << endl;
    }

    if (p == NULL)
    {
        if (DEBUG)
        {
            cout << "Error: Cannot free a NULL pointer." << endl;
        }

        return 0; // Cannot free NULL pointer
    }

    // STEP 2: CALCULATE BLOCK ADDRESS
    // "the pointer p points to the data section of the Node.
    //  Therefore, to find the actual address of the Node, subtract the sizeof(Node) from p."
    //  Debugging guide, page 34

    Node *blockToBeFreed = (Node *)((uintptr_t)p - (uintptr_t)sizeof(Node)); // Get the Node pointer from the user data pointer

    if (DEBUG)
    {
        cout << "Address of the block to be freed: " << blockToBeFreed << endl;
        cout << "Block size: " << blockToBeFreed->size << " bytes" << endl;
    }
    // STEP 3: VALIDATE BLOCK
    if (blockToBeFreed->alloc != 1)
    {
        if (DEBUG)
        {
            cout << "Error: Block is not free." << endl;
        }

        return 0;
    }

    // STEP 4: DETERMINE BLOCK ROW
    // Calculate which FreeList this block belongs to based on its size
    int block_size = blockToBeFreed->size + sizeof(Node); // Total size including the Node header

    int k = 0;
    while (powerOfTwo(k + MIN_ROW_SIZE) < block_size)
    {
        k++; // Find the k that corresponds to the blocksize
    }
    if (DEBUG)
    {
        cout << "BEFORE freeing" << endl;
        printNode(blockToBeFreed, k); // Print the details of the block to be freed
    }
    // STEP 5: mark block as free
    blockToBeFreed->alloc = 0;                                          // Mark the block as free
    blockToBeFreed->size = powerOfTwo(k + MIN_ROW_SIZE) - sizeof(Node); // Set the size of the block to the size of the block minus the Node header

    if (DEBUG)
    {
        cout << "AFTER freeing" << endl;
        printNode(blockToBeFreed, k); // Print the details of the block to be freed
    }

    if (DEBUG)
    {
        cout << "Marking block as free in FreeList[" << k << "]" << endl;
    }

    // STEP 6: COALESCING LOOP
    // "If a block of memory is freed, the system should check whether the buddy
    //  is also free and then combine them together into a single free block."
    // Reference: Week 7 MMBS v2 9.37

    Node *blockToCoalesce = blockToBeFreed; // This is the block that we will coalesce with its buddy if possible
    int current_k = k;

    while (current_k < (Max_Rows - 1)) // Loop until we reach the maximum possible rows
    {
        if (DEBUG)
        {
            cout << "Checking for buddy block in FreeList[" << k << "]" << endl;
        }

        // Finding the buddy block
        // The buddy block is the block that is at the same level as the current block
        // and has the same size, but is located at a different address.
        // The buddy address can be calculated using the formula:
        // buddy = start + (address - start) ^ size where ^ is the XOR operator and start is the start address of the whole memory available.

        Node *buddyAddress = NULL; // Initialize buddyAddress to NULL

        // Finding the buddy block
        // if the block has a start address of address and size of size then the address of the buddy block can be calculated from
        // buddy = start + (address - start) ^ size where ^ is the XOR operator and start is the start address of the whole memory available.
        int sizeOfBlock = powerOfTwo(current_k + MIN_ROW_SIZE);                                                                  // Size of the block in bytes
        Node *startAddress = wholememory;                                                                                        // Start address of the whole memory
        buddyAddress = (Node *)((uintptr_t)startAddress + ((uintptr_t)blockToCoalesce - (uintptr_t)startAddress) ^ sizeOfBlock); // Calculate the buddy address
        int expectedSize = powerOfTwo(current_k + MIN_ROW_SIZE) - sizeof(Node);                                                  // Expected size of the data
        if (DEBUG)
        {
            cout << "Calculating buddy address..." << endl;
            cout << "Buddy address calculated: " << buddyAddress << endl;
            cout << "Size of block: " << sizeOfBlock << " bytes" << endl;
            cout << "Start address of whole memory: " << startAddress << endl;
            cout << "Block to coalesce address: " << blockToCoalesce << endl;
        }
        if (buddyAddress == NULL)
        {
            if (DEBUG)
            {
                cout << "Error: Buddy address is NULL." << endl;
            }
            break; // Buddy address is NULL, cannot coalesce
        }

        if (buddyAddress < wholememory)
        {
            if (DEBUG)
            {
                cout << "Error: Buddy address is less than the start address of the whole memory." << endl;
            }
            break; // Buddy address is invalid, cannot coalesce
        }

        if (buddyAddress >= (Node *)((uintptr_t)wholememory + MEMORYSIZE))
        {
            if (DEBUG)
            {
                cout << "Error: Buddy address is greater than the end address of the whole memory." << endl;
            }
            break; // Buddy address is invalid, cannot coalesce
        }

        if (expectedSize != buddyAddress->size) // Check if they are the same size
        {
            if (DEBUG)
            {
                cout << "Error: Buddy address size does not match the size of the block." << endl;
                cout << "Buddy address size: " << buddyAddress->size + sizeof(Node) << " bytes" << endl;
                cout << "Size of Block: " << expectedSize << " bytes" << endl;
            }
            break; // Buddy address size does not match the size of the block, cannot coalesce
        }
        if (buddyAddress->alloc != 0)
        {
            if (DEBUG)
            {
                cout << "Error: Buddy address is allocated." << endl;
                break;
            }
        }
        if (buddyAddress->previous == NULL)
        { // Remove from head of the FreeList
            FreeList[current_k] = buddyAddress->next;
            if (FreeList[current_k] != NULL)
            {
                FreeList[current_k]->previous = NULL;
            }

            if (DEBUG)
            {
                cout << "Buddy address is the head of the FreeList[" << current_k << "], removing it." << endl;
            }
        }
        else
        {
            if (DEBUG)
            {
                cout << "Remove from middle/tail" << endl;
            }

            if (buddyAddress->previous != NULL)
            {
                Node *previousNode = buddyAddress->previous; // Get the previous node
                Node *nextNode = buddyAddress->next;         // Get the next node

                // To remove from the list, we need to link the nodes wither side of the removed node!
                if (previousNode != NULL)
                {
                    previousNode->next = nextNode; // Link the previous node to the next node
                }

                if (nextNode != NULL)
                {
                    nextNode->previous = previousNode; // Link the next node to the previous node
                }

                // Clean up by setting the removed buddy's pointers to NULL
                buddyAddress->next = NULL;
                buddyAddress->previous = NULL;
            }
        }

        // Colescing logic

        // Determine the blocks address
        if (blockToCoalesce > buddyAddress)
        {
            blockToCoalesce = buddyAddress; // Swap to lower address
        }
        // If we reach here, it means we have a valid buddy block
        if (DEBUG)
        {
            cout << "==================Buddy Coalesce NODE INFO====================\n";
            cout << "Looking good, buddy address is valid." << endl;
            cout << "Buddy address: " << buddyAddress << endl;
            printNode(buddyAddress, current_k); // Print the details of the buddy block
            cout << "Block to coalesce address: " << blockToCoalesce << endl;
            printNode(blockToCoalesce, current_k); // Print the details of the block to coalesce
            cout << "==================END Buddy Coalesce NODE INFO================\n";
        }

        // Now we can coalesce the blocks
        if (DEBUG)
        {
            cout << "Coalescing blocks at addresses: " << blockToCoalesce << " and " << buddyAddress << endl;
        }

        current_k++;                                                       // Move to the next size class
        int newSize = powerOfTwo(current_k + MIN_ROW_SIZE) - sizeof(Node); // Calculate the new size of the coalesced block
        if (DEBUG)
        {
            cout << "New size of the coalesced block: " << newSize << " bytes" << endl;
        }
        // Set the size of the coalesced block
        blockToCoalesce->size = newSize;  // Add the size of the buddy block and the Node header or we could use the expectedSize variable
        blockToCoalesce->alloc = 0;       // Mark the block as free
        blockToCoalesce->next = NULL;     // Set next to NULL as it is not in the FreeList anymore
        blockToCoalesce->previous = NULL; // Set previous to NULL as it is not in the FreeList anymore
    }
    // Add the coalesced block to the FreeList
    insertToFreeList(blockToCoalesce, current_k); // Insert the coalesced block into the FreeList

    if (DEBUG)
    {
        cout << "Coalesced block inserted into FreeList[" << current_k << "]" << endl;
        printNode(blockToCoalesce, current_k); // Print the details of the coalesced block
    }
    return 1; // dummy statement only
}

void initBuddySys()
{
    if (DEBUG)
    {
        cout << "\n==============Initializing Buddy System===============" << endl;
        cout << "With MEMORYSIZE: " << MEMORYSIZE << " bytes and MIN_BLOCK_SIZE: " << MIN_BLOCK_SIZE << " bytes." << endl;
    }

    // Calculate the maximum number of rows based on MEMORYSIZE and MIN_BLOCK_SIZE
    int Max_Rows = getRows(MEMORYSIZE, MIN_BLOCK_SIZE);

    if (Max_Rows > MAX_POSSIBLE_ROWS)
    {
        if (DEBUG)
        {
            cout << "Error: The number of rows exceeds the maximum possible rows. Defaulting to " << MAX_POSSIBLE_ROWS << endl;
        }

        Max_Rows = MAX_POSSIBLE_ROWS; // Ensure we do not exceed the maximum possible rows
    }

    // Init the list to NULL
    for (int i = 0; i < Max_Rows; i++)
    {

        FreeList[i] = NULL; // Initialize the array of Nodes to NULL
    }

    int largestRow = Max_Rows - 1;      // The largest row corresponds to the maximum block size
    FreeList[largestRow] = wholememory; // Init wholeMem as the largest block in the FreeList
    if (DEBUG)
    {
        cout << "Max Rows: " << Max_Rows << endl;
        cout << "Largest Row: " << largestRow << endl;
        cout << "Initializing FreeList with " << Max_Rows << " rows." << endl;
    }

    // Place the initial memory block in the largest free list
    if (wholememory != NULL)
    {

        FreeList[largestRow] = wholememory;
        wholememory->size = MEMORYSIZE - sizeof(Node); // Data size only
        wholememory->alloc = 0;                        // Free
        wholememory->next = NULL;
        wholememory->previous = NULL;

        if (DEBUG)
        {
            cout << "Placed initial block (" << MEMORYSIZE << " bytes) in FreeList[" << largestRow << "]" << endl;
            cout << "Initial block data size: " << wholememory->size << " bytes" << endl;
        }
    }
    else
    {
        if (DEBUG)
            cout << "Error: wholememory is NULL - cannot initialize buddy system!" << endl;
    }

    if (DEBUG)
        cout << "=== Buddy System Initialization Complete ===" << endl;
}

int getRows(int max, int min)
{
    int p = 1, p2 = 1;
    int counterOfTwo = 0, secondCounterOfTwo = 0;
    int rows = 0;

    // Find k where 2^k >= max
    while (p < max)
    {
        p *= 2;
        counterOfTwo++;
    }

    // Find j where 2^j >= min
    while (p2 < min)
    {
        p2 *= 2;
        secondCounterOfTwo++;
    }

    // Calculate number of rows
    if (counterOfTwo >= secondCounterOfTwo)
    {
        rows = counterOfTwo - secondCounterOfTwo + 1;
    }
    if (DEBUG)
    {
        cout << "Max: " << max << ", Min: " << min << endl;
        cout << "Counter of Two(max): " << counterOfTwo << ", Second Counter of Two(min): " << secondCounterOfTwo << endl;
        cout << "Calculated Rows: " << rows << endl;
    }

    return rows;
}