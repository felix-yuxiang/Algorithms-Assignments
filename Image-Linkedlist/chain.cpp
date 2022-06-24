#include "chain.h"
#include <cmath>
#include <iostream>

// PA1 functions

/**
 * Destroys the current Chain. This function should ensure that
 * memory does not leak on destruction of a chain.
 */
Chain::~Chain() {
  clear();
  delete head_;
  head_=NULL;
}

/**
 * Inserts a new node after the node pointed to by p in the
 * chain (so p->next is the new node) and returns a pointer to
 * the newly created node.
 * If p is NULL, inserts a new head node to the chain.
 * This function **SHOULD** create a new Node and increase length_.
 *
 * @param p = The new node should be pointed to by p->next.
 *            If p is NULL, the new node becomes the head of the chain.
 * @param ndata = The data to be inserted.
 */
Chain::Node *Chain::insertAfter(Node *p, const Block &ndata) {
  length_++;
  if (head_ == NULL)
  {
    Node *head = new Node(ndata);
    head_ = head;
    return head;
  }

  if (p == NULL)
  {
    Node *temp = head_;
    Node *head = new Node(ndata);
    head_ = head;
    head->next = temp;
    temp->prev = head;
    return head;
  }
  Node *temp = p->next;
  Node *insertNode = new Node(ndata);
  p->next = insertNode;
  insertNode->prev = p;
  if (temp)
  {
    insertNode->next = temp;
    temp->prev = insertNode;
  }

  return insertNode;
}

/**
 * Swaps the position in the chain of the two nodes pointed to
 * by p and q.
 * If p or q is NULL or p==q, do nothing.
 * Change the chain's head pointer if necessary.
 */
void Chain::swap(Node *p, Node *q) {
  if (p == NULL || q == NULL || p == q)
    return;
  Node *p_pre_orig = p->prev;
  Node *p_pos_orig = p->next;
  Node *q_pre_orig = q->prev;
  Node *q_pos_orig = q->next;
  // Handle the head pointer
  if (!p_pre_orig)
  {
    head_ = q;
  }
  else if (!q_pre_orig)
  {
    head_ = p;
  }
  // If p and q are neighbors
  if (q_pre_orig == p)
  {
    q->prev = p_pre_orig;
    p->prev = q;
    p->next = q_pos_orig;
    q->next = p;
    if (p_pre_orig)
      p_pre_orig->next = q;
    if (q_pos_orig)
      q_pos_orig->prev = p;
    return;
  }
  else if (p_pre_orig == q)
  {
    q->prev = p;
    p->prev = q_pre_orig;
    p->next = q;
    q->next = p_pos_orig;
    if (p_pos_orig)
      p_pos_orig->prev = q;
    if (q_pre_orig)
      q_pre_orig->next = p;
    return;
  }

  // If p and q are separated by one or many blocks
  p->prev = q_pre_orig;
  p->next = q_pos_orig;
  q->prev = p_pre_orig;
  q->next = p_pos_orig;
  if (p_pre_orig)
    p_pre_orig->next = q;
  if (q_pos_orig)
    q_pos_orig->prev = p;
  if (p_pos_orig)
    p_pos_orig->prev = q;
  if (q_pre_orig)
    q_pre_orig->next = p;
}

/**
 * Destroys all dynamically allocated memory associated with the
 * current Chain class.
 */
void Chain::clear() {
  while(head_->next){
    Node* nextNodeAfterDelete = head_->next->next;
    if (nextNodeAfterDelete)
    nextNodeAfterDelete->prev = head_;
    delete head_->next;
    head_->next = nextNodeAfterDelete;
    length_--;
  }
}

/**
 * Makes the current object into a copy of the parameter:
 * All member variables should have the same value as
 * those of other, but the memory should be completely
 * independent. This function is used in both the copy
 * constructor and the assignment operator for Chains.
 */
void Chain::copy(Chain const &other) {
  length_ = 0;
  Node* ptr = NULL;
  Node* copyHead = other.head_;
  for (int i=0; i<other.length_; i++) {
    Block b = copyHead->data;
    ptr = insertAfter(ptr, b);
    copyHead = copyHead->next;
  }
}

/* Modifies the current chain: 
 * 1) Find the node with the first (leftmost) block in the unscrambled
 *    image and move it to the head of the chain.
 *	This block is the one whose closest match (to the left) is the
 *	largest.  That is, the distance (using distanceTo) to this block 
 *	is big for all other blocks.
 *	For each block B, find the distanceTo B from every other block
 *	and take the minimum of these distances as B's "value".
 *	Choose the block B with the maximum value over all blocks and
 *	swap its node to the head of the chain.
 *
 * 2) Starting with the (just found) first block B, find the node with
 *    the block that is the closest match to follow B (to the right)
 *    among the remaining blocks, move (swap) it to follow B's node,
 *    then repeat to unscramble the chain/image.
 */
void Chain::unscramble() {
  Node *maxValHead;
  Node *head = head_;
  while (head) {
    maxValHead = findBlockB(head);
    swap(head, maxValHead);
    head = maxValHead;
    head = head->next;
  }
}


//Helper function
Chain::Node *Chain::findBlockB(Node *node) {
  Node* goal = NULL;
  int flag = 0;
  double max = -1;
    while (node) {
      double min = 999999;
      Node *curr = node;
      int temp = flag;
      while (node->prev && temp!=0) {
        double valPre = node->prev->data.distanceTo(curr->data);
        if (valPre <= min)
          min = valPre;
        node = node->prev;
        temp--;
      }
      node = curr;
      while (node->next) {
        double valPos = node->next->data.distanceTo(curr->data);
        if (valPos <= min)
          min = valPos;
        node = node->next;
      }
      node = curr;
      if (min > max) {
        goal = node;
        max = min;
      }

      node = node->next;
      flag++;
    }
    return goal;
} 
