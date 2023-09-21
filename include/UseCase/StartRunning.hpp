#pragma once

#include "Domain/Daruma.hpp"
#include "Domain/Layout/Layout.hpp"

namespace VGG
{
class StartRunning
{
  std::shared_ptr<Layout::Layout> m_layout;

public:
  StartRunning(std::shared_ptr<Daruma> model)
    : m_layout{ new Layout::Layout{ model } }
  {
  }

  void layout(Layout::Size size)
  {
    m_layout->layout(size);
  }

  std::shared_ptr<Layout::Layout> layout()
  {
    return m_layout;
  }

  std::shared_ptr<LayoutNode> layoutTree()
  {
    return m_layout->layoutTree();
  }
};

} // namespace VGG