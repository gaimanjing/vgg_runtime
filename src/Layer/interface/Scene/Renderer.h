#pragma once
#include "Common/Config.h"
#include "Core/PaintNode.h"
#include "core/SkCanvas.h"
#include "core/SkColor.h"

namespace VGG
{

inline void PrintSkMatrix(const SkMatrix& m)
{
  for (int i = 0; i < 9; i++)
  {
    std::cout << m[i] << " ";
  }
  std::cout << std::endl;
}

inline void PrintCurrentTranslation(SkCanvas* canvas, int indent)
{
  auto m = canvas->getTotalMatrix();
  SkVector v;
  m.mapVector(0, 0, &v);
  for (int i = 0; i < indent; i++)
  {
    std::cout << "\t";
  }
  std::cout << v.x() << " " << v.y() << std::endl;
  std::cout << std::endl;
}
class VGG_EXPORTS SkiaRenderer
{
public:
  void draw(SkCanvas* canvas, PaintNode* root)
  {
    canvas->save();
    canvas->scale(1, -1); // convert the whole root to canvas coords
    root->renderPass(canvas);
    canvas->restore();
  }
};

} // namespace VGG