#define RAM_START 0
#define RAM_END 4096

typedef unsigned int uint32;

typedef struct _Node
{
    uint32 size;
    void *next;
} Node;

Node *head;

int kMemInit()
{
    head = (Node *)RAM_START;
    head->size = RAM_END - sizeof(Node);
    head->next = RAM_END;

    return 0;
}

void *kMemAlloc(uint32 size)
{
    // sizeofBlock is size of user requested block with header
    uint32 sizeOfBlock = sizeof(Node) + size;
    if (sizeOfBlock > RAM_END)
        return 0;

    Node *prev = head;
    Node *curr;
    while (prev != RAM_END)
    {
        curr = prev->next;
        // case when there is no fragmentation
        // update to check if its first block?
        if (curr == RAM_END)
        {
            if (prev != RAM_START)
                return 0;

            Node *newNode = prev + sizeOfBlock;
            newNode->next = RAM_END;
            newNode->size = prev->size - sizeOfBlock;

            prev->size = size;

            // update head of free list
            // blc we gave out beginning of free list
            head = newNode;

            curr = prev;
            return curr;
        }

        // if current free block can accomodate
        // issue with head node, missing case when first block, need to update head
        if (prev->size >= size)
        {
            // newNode points to start of updated free block
            // we removed user requested size from current free block
            Node *newNode = prev + sizeOfBlock;
            newNode->next = prev->next;
            newNode->size = prev->size - sizeOfBlock;

            if (prev == head)
            {
                head = newNode;
            }

            prev->next = newNode;

            prev->size = size;
            return prev;
        }
        // update prev to move fwd if current free block can't accomodate
        prev = curr;
    }
}

int kMemDealloc(Node *ptr)
{
    Node *prev = head;
    Node *curr = prev->next;
    if (!curr)
    {
    }

    // find right place (address) to insert in free list
    while (curr < ptr)
    {
        // check if neighbors, merge prev & curr
        if (prev + prev->size + sizeof(Node) == curr)
        {
            prev->size += sizeof(Node) + curr->size;
            prev->next = curr->next;
            curr = prev->next;
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }

    // insert ptr (memory we freed) in free list
    prev->next = ptr;
    ((Node *)ptr)->next = curr;

    // check if neighbors to next free block, merge ptr & curr
    if (ptr + sizeof(Node) + ptr->size == curr)
    {
        ptr->size += (sizeof(Node) + curr->size);
        ptr->next = curr->next;
    }

    // check if neighbors to prev free block, merge prev & ptr
    if (prev + sizeof(Node) + prev->size == ptr)
    {
        prev->size += (sizeof(Node) + ptr->size);
        prev->next = ptr->next;
    }

    return 0;
}