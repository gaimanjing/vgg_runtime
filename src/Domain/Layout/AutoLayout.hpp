#pragma once

#include "Rect.hpp"
#include "Rule.hpp"

#include "flexbox_node.h"
#include "grid_layout.h"

#include <memory>

namespace VGG
{
class LayoutNode;

namespace Layout
{

namespace Internal
{

namespace Rule
{
struct GridLayout;
}

class AutoLayout
{
private:
  std::unique_ptr<flexbox_node> m_flexNode;
  flexbox_node* m_flexNodePtr{ nullptr };

  std::unique_ptr<grid_layout> m_gridContainer;
  grid_layout* m_gridContainerPtr{ nullptr };

  std::shared_ptr<grid_item> m_gridItem;
  decltype(m_gridContainerPtr->calc_layout(-1, -1)) m_gridItemFrames;

  bool m_isContainer{ false };

public:
  std::weak_ptr<LayoutNode> view;
  std::weak_ptr<Rule::Rule> rule;

  VGG::Layout::Rect frame;

  bool isIncludedInLayout{ true };

public:
  void configure(bool reset);
  void applyLayout(bool preservingOrigin);
  void frameChanged();

  bool isLeaf();
  bool isEnabled()
  {
    return !rule.expired();
  }
  bool isContainer()
  {
    return m_isContainer;
  }

  std::unique_ptr<flexbox_node>& takeFlexNode()
  {
    return m_flexNode;
  }
  flexbox_node* getFlexNode()
  {
    return m_flexNodePtr;
  }

  std::unique_ptr<grid_layout>& takeGridContainer()
  {
    return m_gridContainer;
  }
  grid_layout* getGridContainer()
  {
    return m_gridContainerPtr;
  }
  std::shared_ptr<grid_item>& getGridItem()
  {
    return m_gridItem;
  }
  auto& gridItemFrames()
  {
    return m_gridItemFrames;
  }

private:
  Size calculateLayout(Size size);
  void configureFlexNodeSize();
  void configureGridItemSize();

  flexbox_node* createFlexNode();

  void configureFlexContainer(Rule::FlexboxLayout* layout);
  void configureFlexItem(Rule::FlexboxItem* layout);

  void configureGridContainer(Rule::GridLayout* layout);
  void configureGridItem(Rule::GridItem* layout);
};

} // namespace Internal
} // namespace Layout
} // namespace VGG