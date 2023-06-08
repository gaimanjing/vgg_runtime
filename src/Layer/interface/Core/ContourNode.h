#pragma once
#include "Common/Config.h"
#include "Core/VGGType.h"
#include "Core/VGGUtils.h"
#include "Core/PaintNode.h"
#include "Attrs.h"
#include "SkiaBackend/SkiaImpl.h"

#include <vector>

namespace VGG
{

struct Contour : public std::vector<PointAttr>
{
  bool closed = true;
  EBoolOp blop;
};

using ContourPtr = std::shared_ptr<Contour>;

class VGG_EXPORTS ContourNode final : public PaintNode
{
  ContourPtr data{ nullptr };

public:
  ContourNode(const std::string& name, ContourPtr data)
    : PaintNode(name, VGG_CONTOUR)
    , data(std::move(data))
  {
  }
  Contour* contour() const
  {
    return data.get();
  }

  Mask asOutlineMask(const glm::mat3* mat)
  {
    Mask mask;
    if (data)
    {
      mask.outlineMask = getSkiaPath(*data, data->closed);
    }
    if (mat)
    {
      mask.outlineMask.transform(toSkMatrix(*mat));
    }
    return mask;
  }
};
}; // namespace VGG