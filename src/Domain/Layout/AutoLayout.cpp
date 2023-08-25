#include "AutoLayout.hpp"

#include "Log.h"
#include "View.hpp"

#include <memory>
#include <vector>

namespace VGG
{
class LayoutView;

namespace Layout
{

namespace Internal
{

namespace
{
using Views = std::vector<std::shared_ptr<LayoutView>>;

void removeAllChildren(std::shared_ptr<flexbox_node> node)
{
  while (node->child_count() > 0)
  {
    node->remove_child(node->child_count() - 1);
  }
}

bool layoutNodeHasExactSameChildren(std::shared_ptr<flexbox_node> node, Views subviews)
{
  if (node->child_count() != subviews.size())
  {
    return false;
  }

  for (auto i = 0; i < subviews.size(); ++i)
  {
    if (node->get_child(i) != subviews[i]->autoLayout()->flexNode)
    {
      return false;
    }
  }

  return true;
}

void attachNodesFromViewHierachy(std::shared_ptr<LayoutView> view)
{
  // todo
  const auto autoLayout = view->autoLayout();
  if (autoLayout->flexNode)
  {
    const auto node = autoLayout->flexNode;
    if (autoLayout->isLeaf())
    {
      removeAllChildren(node);
    }
    else
    {
      std::vector<std::shared_ptr<LayoutView>> subviewsToInclude;
      for (auto subview : view->children())
      {
        if (subview->autoLayout()->isEnabled && subview->autoLayout()->isIncludedInLayout)
        {
          subviewsToInclude.push_back(subview);
        }
      }

      if (!layoutNodeHasExactSameChildren(node, subviewsToInclude))
      {
        removeAllChildren(node);
        for (auto i = 0; i < subviewsToInclude.size(); ++i)
        {
          DEBUG("attachNodesFromViewHierachy, flex node add child");
          node->add_child(subviewsToInclude[i]->autoLayout()->flexNode, i);
        }
      }

      for (auto subview : subviewsToInclude)
      {
        attachNodesFromViewHierachy(subview);
      }
    }
  }
}

void applyLayoutToViewHierarchy(std::shared_ptr<LayoutView> view, bool preserveOrigin)
{
  auto autoLayout = view->autoLayout();
  if (!autoLayout->isIncludedInLayout)
  {
    return;
  }

  if (auto node = autoLayout->flexNode)
  {
    Point origin;
    if (preserveOrigin)
    {
      origin = view->frame().origin;
    }
    Layout::Rect frame = {
      { .x = node->get_layout_left() + origin.x, .y = node->get_layout_top() + origin.y },
      { .width = node->get_layout_width(), .height = node->get_layout_height() }
    };

    DEBUG("applyLayoutToViewHierarchy, view[%p, %s], x=%d, y=%d, width=%d, height=%d",
          view.get(),
          view->path().c_str(),
          static_cast<int>(frame.origin.x),
          static_cast<int>(frame.origin.y),
          static_cast<int>(frame.size.width),
          static_cast<int>(frame.size.height));
    view->setFrame(frame);
  }

  if (!autoLayout->isLeaf())
  {
    for (auto subview : view->children())
    {
      applyLayoutToViewHierarchy(subview, false);
    }
  }

  // todo, gridNode
}

} // namespace

void AutoLayout::applyLayout(bool preservingOrigin)
{
  if (isLeaf())
  {
    return;
  }

  if (auto sharedView = view.lock())
  {
    calculateLayout(sharedView->frame().size);
    applyLayoutToViewHierarchy(sharedView, preservingOrigin);
  }
}

Size AutoLayout::calculateLayout(Size size)
{
  if (auto sharedView = view.lock())
  {
    DEBUG("AutoLayout::calculateLayout, view[%p, %s]",
          sharedView.get(),
          sharedView->path().c_str());
    attachNodesFromViewHierachy(sharedView);
  }

  if (flexNode)
  {
    flexNode->calc_layout();
    return { flexNode->get_layout_width(), flexNode->get_layout_height() };
  }
  else if (gridNode)
  {
    DEBUG("AutoLayout::calculateLayout, grid node calculate");
    gridNode->calc_layout();
    return { TO_VGG_LAYOUT_SCALAR(gridNode->get_layout_width()),
             TO_VGG_LAYOUT_SCALAR(gridNode->get_layout_height()) };
  }
  else
  {
    WARN("AutoLayout::calculateLayout, no layout node, return parameter size");
    return size;
  }
}

bool AutoLayout::isLeaf()
{
  if (isEnabled)
  {
    if (auto sharedView = view.lock())
    {
      for (auto& child : sharedView->children())
      {
        auto autoLayout = child->autoLayout();
        if (autoLayout->isEnabled && autoLayout->isIncludedInLayout)
        {
          return false;
        }
      }
    }
  }

  return true;
}

} // namespace Internal
} // namespace Layout
} // namespace VGG