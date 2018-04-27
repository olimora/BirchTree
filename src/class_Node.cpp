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