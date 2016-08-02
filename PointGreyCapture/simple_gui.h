#pragma once
#include <SFML\Graphics.hpp>
#include <list>
#include <functional>   // std::less
#include <algorithm>    // std::sort, std::includes
#include <memory>



class Node : public sf::Transformable, public sf::Drawable, public std::enable_shared_from_this<Node> {
private:
	static sf::View s_screen;
public:
	static void setScreenView(const sf::View& view) { s_screen = view; }
	static const sf::View& getScreenView() { return s_screen; }
protected:
	mutable bool _reorderChildDirty;
	void resortChildren() const;
	std::list<std::shared_ptr<Node>> _children;
	int _zorder;
	Node* _parent; // weak pointer
	sf::Vector2f _contentSize;
	bool _visible;
public:
	Node() : _reorderChildDirty(false) {}
	virtual ~Node() {}
	void removeFromParent();
	void setParent(Node* node);
	Node* getParent() const { return _parent; }
	int getZOrder() const { return _zorder; }
	void setVisible(bool value) { _visible = value; }
	bool isVisible() const { return _visible; }
	void setZOrder(int zorder);
	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;
	const sf::Vector2f& getContentSize() const { return _contentSize; }
	void setContentSize(const sf::Vector2f& size) { _contentSize = size; }
	sf::Transform getNodeToParentTransform(Node* ancestor = nullptr) const;
	// we use visit to go though drawable nodes, but you override draw_node to draw
	void addChild(Node* node);
	void removeChild(Node* node);

	virtual void update(float dt) ;
	virtual void visit(sf::RenderTarget& target, sf::RenderStates states) const ;
	
	
};