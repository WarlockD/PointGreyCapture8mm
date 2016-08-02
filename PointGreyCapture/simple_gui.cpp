#include "simple_gui.h"
#include <cassert>

using namespace sf;
bool nodeComparisonLess(const std::shared_ptr<Node>& n1, const std::shared_ptr<Node>& n2) { return n1->getZOrder() < n2->getZOrder(); }

void Node::setParent(Node* node) {
	assert(_parent == nullptr);
	assert(this != node);
	if (node == nullptr) { if (_parent) _parent->removeChild(this); }
	else { 
		
		if (_parent) {
			auto lockthis = node->shared_from_this(); // lock it in case we delete it by accdent when we move it
			_parent->removeChild(this);
			node->addChild(this);
		}
		else node->addChild(this);
	}
	_parent = node;
}

sf::Transform Node::getNodeToParentTransform(Node* ancestor ) const {
	sf::Transform t(this->getNodeToParentTransform());
	for (Node *p = _parent; p != nullptr && p != ancestor; p = p->getParent()) t = p->getNodeToParentTransform() * t;
	return t;
}
void Node::setZOrder(int zorder) {
	if(_parent)  _parent->_reorderChildDirty = true;
	_zorder = zorder;
}

void Node::removeFromParent() {
	if (_parent)  _parent->removeChild(this);
}

sf::FloatRect Node::getLocalBounds() const {
	return FloatRect(0.0f, 0.0f, _contentSize.x, _contentSize.y);
}
sf::FloatRect Node::getGlobalBounds() const {
	return getNodeToParentTransform().transformRect(getLocalBounds());
}


void Node::addChild(Node* node) {
	assert(node != nullptr);
	assert(node->_parent == nullptr);
	_children.push_back(node->shared_from_this());
	_reorderChildDirty = true;
	node->_parent = this;
}
void Node::removeChild(Node* node) {
	assert(node != nullptr);
	assert(node->_parent != this);
	node->_parent = nullptr; // set null in case its removed with .erase
	_children.remove(node->shared_from_this()); 
}
void Node::resortChildren() const {
	if (_reorderChildDirty) {
		// since we do this in visit, saves us making _children maulable
		const_cast<std::list<std::shared_ptr<Node>>&>(_children).sort(nodeComparisonLess);
		_reorderChildDirty = false;
	}
}
void Node::update(float dt) {

	for (auto& child : _children) child->update(dt);
	resortChildren();
}

void Node::visit(RenderTarget& target, RenderStates states) const {
	// quick return if not visible. children won't be drawn.
	if (!_visible) return;
	states.transform *= getTransform();
	if (_children.size() > 0) {
		resortChildren();
		auto it = _children.begin();
		for (; it != _children.end(); it++)
		{
			auto& node = (*it);
			if (node && node->_zorder < 0) node->visit(target, states);
			else break;
		}
		// self draw
		draw(target, states);
		for (; it != _children.end(); it++)
		{
			auto& node = (*it);
			if (node) node->draw(target, states);
		}
	} else draw(target, states);
}
