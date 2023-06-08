#pragma once
// #include "Basic/Renderer.hpp"
#include "Common/Config.h"
#include "Core/Node.hpp"
#include "Core/VGGType.h"
#include "Core/Geometry.hpp"
#include "Core/VGGUtils.h"
#include "Core/Attrs.h"
#include "Core/RenderState.h"
#include "Core/Mask.h"
#include "Scene/Scene.h"

#include "glm/matrix.hpp"
#include "core/SkCanvas.h"
#include "core/SkColor.h"
#include "core/SkMatrix.h"
#include "core/SkPaint.h"
#include "pathops/SkPathOps.h"

#include <any>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
namespace VGG
{

class PaintNode__pImpl;
class VGG_EXPORTS PaintNode : public Node
{
  VGG_DECL_IMPL(PaintNode)

protected:
  static SkCanvas* s_defaultCanvas;
  static RenderState* s_renderState;
  Bound2 m_bound;
  glm::mat3 m_transform{ 1.0 };

  std::string guid{};
  std::vector<std::string> maskedBy{};
  Mask outlineMask;
  EMaskType maskType{ MT_None };
  EBoolOp m_clipOperator{ BO_None };

  Style style;
  ContextSetting m_contextSetting;
  ObjectType type;

  bool visible{ true };
  std::optional<VGGColor> bgColor;

  friend class NlohmannBuilder;
  friend class SkiaRenderer;

public:
  PaintNode(const std::string& name, ObjectType type);

  void addChild(const std::shared_ptr<PaintNode> node)
  {
    pushChildBack(std::move(node));
  }

  void setContectSettings(const ContextSetting& settings)
  {
    this->m_contextSetting = settings;
  }

  const ContextSetting& contextSetting() const
  {
    return this->m_contextSetting;
  }

  void setClipOperator(EBoolOp op)
  {
    m_clipOperator = op;
  }

  void setVisible(bool visible)
  {
    this->visible = visible;
  }

  void setBackgroundColor(const VGGColor& color)
  {
    this->bgColor = color;
  }

  bool isVisible() const
  {
    return this->visible;
  }

  EBoolOp clipOperator() const
  {
    return m_clipOperator;
  }

  glm::mat3 mapTransform(const PaintNode* node) const;

  void setLocalTransform(const glm::mat3& transform)
  {
    this->m_transform = transform;
  }

  const glm::mat3& localTransform() const
  {
    // TODO:: if the node is detached from the parent, this transform should be reset;
    return m_transform;
  }

  const Bound2& getBound() const
  {
    return this->m_bound;
  }

  void setBound(const Bound2& bound)
  {
    this->m_bound = bound;
  }

  const std::string& GUID() const
  {
    return guid;
  }

  bool isMasked() const
  {
    return !maskedBy.empty();
  }

  EMaskType getMaskType() const
  {
    return this->maskType;
  }

  /**
   * Return a matrix that transform from this node to the given node
   * */
  virtual SkCanvas* getSkCanvas();
  RenderState* getRenderState();
  void setOutlineMask(const Mask& mask);

  // TODO:: this routine should be removed to a stand alone render pass
  VGG::ObjectTableType PreprocessMask()
  {
    ObjectTableType hash;
    visitNode(this, hash);
    return hash;
  }

  virtual Mask asOutlineMask(const glm::mat3* mat);
  virtual void asAlphaMask();

  ~PaintNode();

public:
  void visitNode(VGG::Node* p, ObjectTableType& table);

  template<typename F>
  void visitFunc(VGG::Node* p, F&& f)
  {
    if (!p)
      return;
    f(static_cast<PaintNode*>(p));
    for (const auto& c : m_firstChild)
    {
      visitFunc(c.get(), std::forward<F>(f));
    }
  }

public:
  // TODO:: chagne the following functions accessbility
  void invokeRenderPass(SkCanvas* canvas)
  {
    if (!visible)
      return;
    preRenderPass(canvas);
    renderOrderPass(canvas);
    postRenderPass(canvas);
  }

  virtual void renderOrderPass(SkCanvas* canvas)
  {
    for (const auto& p : this->m_firstChild)
    {
      auto q = static_cast<PaintNode*>(p.get());
      q->invokeRenderPass(canvas);
    }
  }
  virtual void preRenderPass(SkCanvas* canvas)
  {
    paintPass();
  }

  virtual void postRenderPass(SkCanvas* canvas)
  {
    canvas->restore();
  }

  Mask makeMaskBy(EBoolOp maskOp);

protected:
  virtual void paintPass();
  void renderPass(SkCanvas* canvas); // TODO:: should be private access

  virtual void paintEvent(SkCanvas* canvas);

private:
  void drawDebugBound(SkCanvas* canvas);
};
} // namespace VGG
