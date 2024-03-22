/*
 * Copyright 2023 VeryGoodGraphics LTD <bd@verygoodgraphics.com>
 *
 * Licensed under the VGG License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.verygoodgraphics.com/licenses/LICENSE-1.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "ShapeAttribute.hpp"

#include "Layer/Core/PaintNode.hpp"

namespace VGG::layer
{
Bound ShapeAttribute::onRevalidate()
{
  if (m_paintNode)
  {
    m_shape = m_paintNode->asVisualShape(0);
    // This is a temperoray solution, we don't need to set shape from setShape so far
  }
  if (m_shape.isEmpty())
  {
    return Bound{};
  }
  const auto rect = m_shape.bounds();
  return Bound{ rect.x(), rect.y(), rect.width(), rect.height() };
}

} // namespace VGG::layer