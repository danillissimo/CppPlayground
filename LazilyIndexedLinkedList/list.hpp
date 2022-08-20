#pragma once

#include <memory>

struct OperationHistoryNode;
typedef std::shared_ptr<OperationHistoryNode> HistoryPtr;

typedef int DataType;
struct LinkedListNode;

class IndexedLinkedList;

enum struct OperationType
{
    Addition,
    Removal,
    None,
};

struct OperationHistoryNode
{
  public:
    HistoryPtr    Next      = nullptr;
    OperationType Operation = OperationType::None;
    size_t        Position;
};


// MORE DOUBLY LINKED LISTS FOR THE THRONE OF DOUBLY LINKED LISTS


struct LinkedListNode
{
  public:
    LinkedListNode *Prev = nullptr, *Next = nullptr;
    DataType        Payload;

  private:
    friend IndexedLinkedList;

    size_t     Index;
    HistoryPtr HistoryNode;
};

class IndexedLinkedList
{
  private:
    LinkedListNode *First = nullptr, *Last = nullptr;
    size_t          Count = 0;

    mutable HistoryPtr HistoryEnding;

    OperationHistoryNode * GetNewHistoryNode(OperationType operation) const
    {
        OperationHistoryNode * historyNode = new OperationHistoryNode();
        historyNode->Operation             = operation;
        historyNode->Next                  = nullptr;
        if (HistoryEnding != nullptr)
        {
            HistoryEnding->Next = HistoryPtr(historyNode);
        }
        else
        {
            HistoryEnding = HistoryPtr(historyNode);
        }
    }

  public:
    LinkedListNode * GetFirst() const noexcept { return First; }
    LinkedListNode * GetLast() const noexcept { return Last; }

    size_t GetLength() const noexcept { return Count; }

    void insert(LinkedListNode * node, LinkedListNode * after = nullptr)
    {
        OperationHistoryNode * historyNode = GetNewHistoryNode(OperationType::Addition);

        if (after == nullptr)
        {
            node->Next = First;
            node->Prev = nullptr;

            if (First != nullptr)
            {
                First->Prev = node;
            }
            else
            {
                Last = node;
            }

            First                 = node;
            historyNode->Position = 0;
        }
        else if (after == Last)
        {
            node->Next = nullptr;
            node->Prev = Last;

            Last->Next = node;

            Last                  = node;
            historyNode->Position = Count - 1;
        }
        else
        {
            node->Prev = after;
            node->Next = after->Next;

            after->Next->Prev = node;
            after->Next       = node;

            historyNode->Position = GetIndex(after) + 1;
        }
        node->HistoryNode = HistoryEnding;
        Count++;
    }

    LinkedListNode * cut(LinkedListNode * node)
    {
        OperationHistoryNode * historyNode = GetNewHistoryNode(OperationType::Removal);

        if (node == First)
        {
            First = First->Next;
            if (First != nullptr)
                First->Prev = nullptr;
            historyNode->Position = 0;
        }
        else if (node == Last)
        {
            Last                  = Last->Prev;
            Last->Next            = nullptr;
            historyNode->Position = Count;
        }
        else
        {
            node->Prev->Next = node->Next;
            node->Next->Prev = node->Prev;

            historyNode->Position = GetIndex(node->Prev);
        }
        Count--;
        return node;
    }

    size_t GetIndex(LinkedListNode * node) const
    {
        while (node->HistoryNode.get() != HistoryEnding.get())
        {
            auto ptr = node->HistoryNode.get();
            // Dgaf if something happened at bigger heights
            if (ptr->Position <= node->Index)
            {
                switch (ptr->Operation)
                {
                    case OperationType::Removal: node->Index--; break;
                    case OperationType::Addition: node->Index++; break;
                }
            }

            node->HistoryNode = ptr->Next;
        }
    }

  public:
    static IndexedLinkedList * CreateTestSample()
    {
        auto numElements = 100'000;


        auto result                      = new IndexedLinkedList();
        result->HistoryEnding            = HistoryPtr(new OperationHistoryNode());
        result->HistoryEnding->Next      = nullptr;
        result->HistoryEnding->Operation = OperationType::None;

        result->First              = new LinkedListNode();
        result->First->Prev        = nullptr;
        result->First->HistoryNode = result->HistoryEnding;
        result->First->Payload     = 0;
        result->First->Index       = 0;

        auto lastAdded = result->First;
        for (auto i = 1; i < numElements - 1; i++)
        {
            auto newNode         = new LinkedListNode();
            newNode->Payload     = (i - i / 2) * i;
            newNode->Index       = i;
            newNode->HistoryNode = result->HistoryEnding;
            newNode->Prev        = lastAdded;
            lastAdded->Next      = newNode;

            lastAdded = newNode;
        }

        auto newNode         = new LinkedListNode();
        auto i               = numElements;
        newNode->Payload     = (i - i / 2) * i;
        newNode->Index       = numElements - 1;
        newNode->HistoryNode = result->HistoryEnding;
        newNode->Prev        = lastAdded;
        newNode->Next        = nullptr;

        result->Count = numElements;

        return result;
    }
};