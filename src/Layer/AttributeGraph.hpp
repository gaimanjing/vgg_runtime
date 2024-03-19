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

#include "Layer/Core/Attrs.hpp"
#include "Layer/Core/VNode.hpp"
#include "Layer/Core/VShape.hpp"
// #include "Layer/LayerAttribute.hpp"
#include "Layer/Memory/VNew.hpp"

#include <core/SkPicture.h>

#define VGG_ATTRIBUTE(attrName, attrType, attrContainer)                                           \
  const attrType& get##attrName() const                                                            \
  {                                                                                                \
    return attrContainer;                                                                          \
  }                                                                                                \
  void set##attrName(const attrType& v)                                                            \
  {                                                                                                \
    attrContainer = v;                                                                             \
    this->invalidate();                                                                            \
  }                                                                                                \
  void set##attrName(attrType&& v)                                                                 \
  {                                                                                                \
    attrContainer = std::move(v);                                                                  \
    this->invalidate();                                                                            \
  }

#define VGG_ATTRIBUTE_PTR(attrName, attrType, attrContainer)                                       \
  attrType* get##attrName() const                                                                  \
  {                                                                                                \
    return attrContainer;                                                                          \
  }                                                                                                \
  void set##attrName(attrType* v)                                                                  \
  {                                                                                                \
    attrContainer = v;                                                                             \
    this->invalidate();                                                                            \
  }

#define VGG_CLASS_MAKE(className)                                                                  \
  template<typename... Args>                                                                       \
  static Ref<className> Make(Args&&... args)                                                       \
  {                                                                                                \
    return Ref<className>(V_NEW<className>(std::forward<Args>(args)...));                          \
  }

namespace VGG::layer
{

class Renderer;

class Attribute : public VNode
{
public:
  Attribute(VRefCnt* cnt)
    : VNode(cnt)
  {
  }
  virtual void render(Renderer* renderer){};

private:
};

class ShapeAttribute : public Attribute
{
public:
  ShapeAttribute(VRefCnt* cnt)
    : Attribute(cnt)
  {
  }
  virtual VShape getShape()
  {
    return m_shape;
  }
  void setShape(const VShape& shape)
  {
    m_shape = shape;
    invalidate();
  }

  Bound onRevalidate() override
  {
    const auto rect = m_shape.bounds();
    return Bound{ rect.x(), rect.y(), rect.width(), rect.height() };
  }

private:
  VShape m_shape;
};

class LayerAttribute;
class PaintNode;
class LayerPostProcessAttribute;


class DropShadowAttribute : public Attribute
{
public:
  DropShadowAttribute(VRefCnt* cnt)
    : Attribute(cnt)
  {
  }

  void render(Renderer* renderer) override
  {
  }

  VGG_ATTRIBUTE(DropShadowStyle, std::vector<DropShadow>, m_shadow);
  VGG_CLASS_MAKE(DropShadowAttribute);

private:
  std::vector<DropShadow> m_shadow;
};

class ObjectAttribute;
class InnerShadowAttribute : public Attribute
{
public:
  InnerShadowAttribute(VRefCnt* cnt, ObjectAttribute* shape)
    : Attribute(cnt)
    , m_shape(shape)
  {
  }

  void render(Renderer* renderer) override
  {
  }

  VGG_ATTRIBUTE(InnerShadowStyle, std::vector<InnerShadow>, m_shadow);
  VGG_CLASS_MAKE(InnerShadowAttribute);

private:
  std::vector<InnerShadow> m_shadow;
  ObjectAttribute*         m_shape;
};

class ShapeMaskAttribute : public ShapeAttribute
{
public:
  ShapeMaskAttribute(VRefCnt* cnt)
    : ShapeAttribute(cnt)
  {
  }

  VGG_ATTRIBUTE(MaskID, std::vector<std::string>, m_maskID);
  VGG_CLASS_MAKE(ShapeMaskAttribute);
  VShape getShape() override
  {
    return m_shape;
  }

private:
  VShape                   m_shape;
  std::vector<std::string> m_maskID;
};

} // namespace VGG::layer
