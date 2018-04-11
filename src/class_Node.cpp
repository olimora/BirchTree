//
// Created by omora on 2018-03-24.
//

#include "class_Node.h"
#include "include_global.h"

Vec BNode::getCentroid() {
    return this->cf->getCentroid();
}

NLNode::NLNode() {
    this->cf = std::make_shared<CF>();
    this->entries.reserve(Global::get().getBF_B());
}

LNode::LNode() {
    this->cf = std::make_shared<CF>();
    this->entries.reserve(Global::get().getBF_L());
}

LNode::LNode(Vec point) {
    this->cf = std::make_shared<CF>(1, point, VEC::squaredSum(point));
    this->entries.reserve(Global::get().getBF_L());
    this->entries.push_back(std::make_shared<CF>(1, point, VEC::squaredSum(point)));
}

int NLNode::getBF() { return Global::get().getBF_B(); }
int LNode::getBF() { return Global::get().getBF_L(); }

int LNode::getMemoryConsumed() {
    // memory consumed by LNode
    int sum = sizeof(this->cf) + this->cf->getMemoryConsumed(); // this CF - size of field (ptr) + size of object there
    sum += sizeof(this->entries);                               // this entries - size of field (vector)
    sum += Global::get().getBF_L() * sizeof(this->entries[0]);     // + entries_size * sizeof(ptr)
//    sum += sizeof(this->left) + sizeof(this->right);            // + sizeof(left) + sizeof(right) --those pointers
    sum += sizeof(this);
    return sum;
}

int NLNode::getMemoryConsumed() {
    int sum = sizeof(this->cf) + this->cf->getMemoryConsumed();
    sum += sizeof(this->entries);
    sum += Global::get().getBF_B() * sizeof(this->entries[0]);
    sum += sizeof(this);
    return sum;
}