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
#pragma once
#include "RenderNode.hpp"
#include "AttributeNode.hpp"
#include "ShapeAttribute.hpp"
#include "LayerAttribute.hpp"
#include "ObjectAttribute.hpp"
#include "MaskAttribute.hpp"
#include "AttributeAccessor.hpp"
#include "TransformAttribute.hpp"

#include "Layer/Core/VNode.hpp"

namespace VGG::layer
{

class AttributeAccessor;

class DefaultRenderNode : public RenderNode
{
public:
  DefaultRenderNode(
    VRefCnt*                  cnt,
    Ref<TransformAttribute>   transform,
    Ref<StyleObjectAttribute> styleObject,
    Ref<LayerFXAttribute>     layerPostProcess,
    Ref<AlphaMaskAttribute>   alphaMask,
    Ref<ShapeMaskAttribute>   shapeMask,
    Ref<ShapeAttribute>       shape)
    : RenderNode(cnt, INVALIDATE)
    , m_transformAttr(transform)
    , m_objectAttr(styleObject)
    , m_alphaMaskAttr(alphaMask)
    , m_shapeMaskAttr(shapeMask)
  {
    observe(m_transformAttr);
    observe(m_objectAttr);
    observe(m_alphaMaskAttr);
    observe(m_shapeMaskAttr);
    // observe(m_shapeAttr);
  }
  void render(Renderer* renderer) override;

  bool isInvalid() const
  {
    return VNode::isInvalid();
  }
  Bound onRevalidate() override;

  AttributeAccessor* access()
  {
    return m_accessor.get();
  }

  ~DefaultRenderNode();
  static Ref<DefaultRenderNode> MakeFrom(VAllocator* alloc, PaintNode* node); // NOLINT
private:
  VGG_CLASS_MAKE(DefaultRenderNode);

  SkRect                              recorder(Renderer* renderer);
  std::pair<sk_sp<SkPicture>, SkRect> revalidatePicture(const SkRect& bounds);

  void beginLayer(
    Renderer*            renderer,
    const SkPaint*       paint,
    const VShape*        clipShape,
    sk_sp<SkImageFilter> backdropFilter);

  void                      endLayer(Renderer* renderer);
  Ref<TransformAttribute>   m_transformAttr;
  Ref<StyleObjectAttribute> m_objectAttr;
  Ref<AlphaMaskAttribute>   m_alphaMaskAttr;
  Ref<ShapeMaskAttribute>   m_shapeMaskAttr;
  // Ref<ShapeAttribute>       m_shapeAttr;
  sk_sp<SkPicture>          m_picture;

  std::unique_ptr<AttributeAccessor> m_accessor;
};
} // namespace VGG::layer