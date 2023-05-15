#include "Basic/PaintNode.h"
#include "Basic/PathNode.h"
#include "Basic/VGGType.h"
#include "Basic/VGGUtils.h"
#include "Components/Styles.hpp"
#include "Utils/Utils.hpp"
#include "include/core/SkBlendMode.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPaint.h"
#include "include/core/SkShader.h"
#include "include/core/SkSurface.h"
#include "include/core/SkTileMode.h"
#include "include/core/SkTypes.h"
#include "include/effects/SkRuntimeEffect.h"
#include "include/core/SkColor.h"
#include "include/core/SkPath.h"
#include "include/gpu/GrTypes.h"
#include "include/pathops/SkPathOps.h"
#include <limits>

namespace VGG
{
constexpr float EPS = std::numeric_limits<float>::epsilon();
double calcRadius(double r0,
                  const glm::vec2& p0,
                  const glm::vec2& p1,
                  const glm::vec2& p2,
                  glm::vec2* left,
                  glm::vec2* right)
{
  glm::vec2 a = p0 - p1;
  glm::vec2 b = p2 - p1;
  double alen = a.length();
  double blen = b.length();
  if (std::fabs(alen) < EPS || std::fabs(blen) < EPS)
  {
    return 0.;
  }
  ASSERT(alen > 0 && blen > 0);
  double cosTheta = glm::dot(a, b) / alen / blen;
  if (cosTheta + 1 < EPS) // cosTheta == -1
  {
    if (left)
    {
      left->x = p1.x;
      left->y = p1.y;
    }
    if (right)
    {
      right->x = p1.x;
      right->y = p1.y;
    }
    return r0;
  }
  else if (1 - cosTheta < EPS) // cosTheta == 1
  {
    return 0.;
  }
  double tanHalfTheta = std::sqrt((1 - cosTheta) / (1 + cosTheta));
  double radius = r0;
  radius = std::min(radius, 0.5 * alen * tanHalfTheta);
  radius = std::min(radius, 0.5 * blen * tanHalfTheta);
  if (left)
  {
    ASSERT(tanHalfTheta > 0);
    float len = radius / tanHalfTheta;
    *left = p1 + float(len / alen) * a;
  }
  if (right)
  {
    ASSERT(tanHalfTheta > 0);
    double len = radius / tanHalfTheta;
    *right = p1 + (float(len / blen) * b);
  }
  return radius;
}

SkPath getSkiaPath(const std::vector<PointAttr>& points, bool isClosed)
{
  constexpr float w = 1.0;
  constexpr float h = 1.0;
  auto& pts = points;

  ASSERT(w > 0);
  ASSERT(h > 0);

  SkPath skPath;

  if (pts.size() < 2)
  {
    // WARN("Too few path points.");
    return skPath;
  }

  using PM = PointMode;
  auto* startP = &pts[0];
  auto* endP = &pts[pts.size() - 1];
  auto* prevP = endP;
  auto* currP = startP;
  auto* nextP = currP + 1;

  const glm::vec2 s = { w, h };

  if (currP->radius > 0 && currP->mode() == PM::STRAIGHT)
  {
    glm::vec2 start = currP->point * s;
    calcRadius(currP->radius,
               prevP->point * s,
               currP->point * s,
               nextP->point * s,
               nullptr,
               &start);
    skPath.moveTo(start.x, start.y);
  }
  else
  {
    skPath.moveTo(w * currP->point.x, h * currP->point.y);
  }

  while (true)
  {
    if (currP->mode() == PM::STRAIGHT && nextP->mode() == PM::STRAIGHT)
    {
      if (nextP->radius > 0 && nextP->mode() == PM::STRAIGHT)
      {
        auto* next2P = (nextP == endP) ? startP : (nextP + 1);
        auto next2Pp = next2P->to.has_value() ? next2P->to.value() : next2P->point;
        double r = calcRadius(nextP->radius, currP->point * s, nextP->point * s, next2Pp * s, 0, 0);
        skPath.arcTo(w * nextP->point.x, h * nextP->point.y, w * next2Pp.x, h * next2Pp.y, r);
      }
      else
      {
        skPath.lineTo(w * nextP->point.x, h * nextP->point.y);
      }
    }
    else if (currP->mode() == PM::DISCONNECTED && nextP->mode() == PM::DISCONNECTED)
    {
      bool hasFrom = currP->from.has_value();
      bool hasTo = nextP->to.has_value();
      if (!hasFrom && !hasTo)
      {
        skPath.lineTo(w * nextP->point.x, h * nextP->point.y);
      }
      else if (hasFrom && !hasTo)
      {
        auto& from = currP->from.value();
        skPath.quadTo(w * from.x, h * from.y, w * nextP->point.x, h * nextP->point.y);
      }
      else if (!hasFrom && hasTo)
      {
        auto& to = nextP->to.value();
        skPath.quadTo(w * to.x, h * to.y, w * nextP->point.x, h * nextP->point.y);
      }
      else
      {
        auto& from = currP->from.value();
        auto& to = nextP->to.value();
        skPath.cubicTo(w * from.x,
                       h * from.y,
                       w * to.x,
                       h * to.y,
                       w * nextP->point.x,
                       h * nextP->point.y);
      }
    }
    else if (currP->mode() != PM::STRAIGHT && nextP->mode() != PM::STRAIGHT)
    {
      if ((currP->mode() == PM::DISCONNECTED && !currP->from.has_value()) ||
          (nextP->mode() == PM::DISCONNECTED && !nextP->to.has_value()) ||
          (currP->mode() != PM::DISCONNECTED &&
           !(currP->from.has_value() && currP->to.has_value())) ||
          (nextP->mode() != PM::DISCONNECTED &&
           !(nextP->from.has_value() && nextP->to.has_value())))
      {
        WARN("Missing control points.");
        return skPath;
      }
      auto& from = currP->from.value();
      auto& to = nextP->to.value();
      skPath.cubicTo(w * from.x,
                     h * from.y,
                     w * to.x,
                     h * to.y,
                     w * nextP->point.x,
                     h * nextP->point.y);
    }
    else if (currP->mode() == PM::STRAIGHT && nextP->mode() != PM::STRAIGHT)
    {
      if (!nextP->to.has_value())
      {
        skPath.lineTo(w * nextP->point.x, h * nextP->point.y);
      }
      else
      {
        auto& to = nextP->to.value();
        skPath.quadTo(w * to.x, h * to.y, w * nextP->point.x, h * nextP->point.y);
      }
    }
    else if (currP->mode() != PM::STRAIGHT && nextP->mode() == PM::STRAIGHT)
    {
      if (nextP->radius > 0 && nextP->mode() == PM::STRAIGHT)
      {
        auto* next2P = (nextP == endP) ? startP : (nextP + 1);
        if (!currP->from.has_value())
        {
          glm::vec2 start;
          double r = calcRadius(nextP->radius,
                                currP->point * s,
                                nextP->point * s,
                                next2P->point * s,
                                &start,
                                nullptr);
          skPath.lineTo(start.x, start.y);
          skPath.arcTo(w * nextP->point.x,
                       h * nextP->point.y,
                       w * next2P->point.x,
                       h * next2P->point.y,
                       r);
        }
        else
        {
          auto currPfrom = currP->from.value();
          constexpr float radius_coeff = 1.0;
          // glm::vec2 p =
          // currP->point.add(currPfrom.sub(currP->point).scale(radius_coeff)).scale(w, h);
          glm::vec2 p = (currP->point + radius_coeff * (currPfrom - currP->point)) * s;
          glm::vec2 start;
          double r =
            calcRadius(nextP->radius, p, nextP->point * s, next2P->point * s, &start, nullptr);
          skPath.quadTo(p.x, p.y, start.x, start.y);
          skPath.arcTo(w * nextP->point.x,
                       h * nextP->point.y,
                       w * next2P->point.x,
                       h * next2P->point.y,
                       r);
        }
      }
      else
      {
        if (!currP->from.has_value())
        {
          skPath.lineTo(w * nextP->point.x, h * nextP->point.y);
        }
        else
        {
          auto& from = currP->from.value();
          skPath.quadTo(w * from.x, h * from.y, w * nextP->point.x, h * nextP->point.y);
        }
      }
    }
    else
    {
      WARN("Invalid point mode combination: %d %d", (int)currP->mode(), (int)nextP->mode());
    }
    currP = nextP;
    nextP = (nextP == endP) ? startP : (nextP + 1);

    if (isClosed)
    {
      if (currP == startP)
      {
        break;
      }
    }
    else
    {
      if (nextP == startP)
      {
        break;
      }
    }
  }

  if (isClosed)
  {
    skPath.close();
  }

  return skPath;
}

// void drawPathFill(SkCanvas* canvas,
//                   const Frame& frame,
//                   const SkPath& skPath,
//                   const FillStyle* style,
//                   double globalAlpha)
// {
//   ASSERT(canvas);
//   SkPaint fillPen;
//   fillPen.setStyle(SkPaint::kFill_Style);
//
//   if (!style)
//   {
//     canvas->drawPath(skPath, fillPen);
//   }
//   else if (FillTypeChooser<FillStyle>::isFlat(*style))
//   {
//     fillPen.setColor(style->color);
//     fillPen.setAlphaf(fillPen.getAlphaf() * globalAlpha);
//     canvas->drawPath(skPath, fillPen);
//   }
//   else if (FillTypeChooser<FillStyle>::isLinearGradient(*style))
//   {
//     fillPen.setShader(style->gradient.getLinearShader(frame));
//     fillPen.setAlphaf(style->contextSettings.opacity * globalAlpha);
//     canvas->drawPath(skPath, fillPen);
//   }
//   else if (FillTypeChooser<FillStyle>::isRadialGradient(*style))
//   {
//     fillPen.setShader(style->gradient.getRadialShader(frame));
//     fillPen.setAlphaf(style->contextSettings.opacity * globalAlpha);
//     canvas->drawPath(skPath, fillPen);
//   }
//   else if (FillTypeChooser<FillStyle>::isAngularGradient(*style))
//   {
//     fillPen.setShader(style->gradient.getAngularShader(frame));
//     fillPen.setAlphaf(style->contextSettings.opacity * globalAlpha);
//     canvas->drawPath(skPath, fillPen);
//   }
//   else if (style->fillType == FillStyle::FillType::IMAGE)
//   {
//     if (auto& name = style->imageName)
//     {
//       fillPen.setShader(style->getImageShader(frame));
//       fillPen.setAlphaf(style->contextSettings.opacity * globalAlpha);
//       canvas->drawPath(skPath, fillPen);
//     }
//   }
// }

// bool maskedByOthers = !maskedBy.empty();
// auto cvs = canvas;
// sk_sp<SkSurface> objectTarget;
// if (maskedByOthers)
// {
//   objectTarget = SkSurface::MakeRenderTarget(canvas->recordingContext(),
//                                              SkBudgeted::kYes,
//                                              canvas->imageInfo(),
//                                              0,
//                                              GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin,
//                                              &canvas->getSurface()->props(),
//                                              false);
//   if (objectTarget)
//   {
//     cvs = objectTarget->getCanvas();
//   }
//   else
//   {
//     WARN("target cannot be created\n");
//   }
// }
//
// SkPaint p;
// p.setShader(shader);
// cvs->drawRect(toSkRect(this->bound), p);

// if (maskedByOthers)
// {
//   auto target = SkSurface::MakeRenderTarget(canvas->recordingContext(),
//                                             SkBudgeted::kYes,
//                                             canvas->imageInfo(),
//                                             0,
//                                             GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin,
//                                             &canvas->getSurface()->props(),
//                                             false);
//
//   SkPaint fill;
//   fill.setColor4f({ 1, 0, 0, 1 });
//   fill.setStyle(SkPaint::kFill_Style);
//   auto c = target->getCanvas();
//   c->concat(canvas->getTotalMatrix());
//   c->drawRect(toSkRect(this->bound), fill);
//
//   // using shader
//   if (objectTarget)
//   {
//     SkSamplingOptions opt;
//     sk_sp<SkShader> objectShader =
//       objectTarget->makeImageSnapshot()->makeShader(SkTileMode::kClamp,
//                                                     SkTileMode::kClamp,
//                                                     opt,
//                                                     nullptr);
//     auto maskShader = target->makeImageSnapshot()->makeShader(SkTileMode::kClamp,
//                                                               SkTileMode::kClamp,
//                                                               opt,
//                                                               nullptr);
//
//     const char* sksl = "uniform shader input_1;"
//                        "uniform shader input_2;"
//                        "uniform vec4 rect;"
//                        "half4 main(float2 coord) {"
//                        "  return sample(input_2, coord);"
//                        "}";
//
//     // Create SkShader from SkSL, then fill surface: // SK_FOLD_START
//
//     // Create an SkShader from our SkSL, with `children` bound to the inputs:
//     auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(sksl));
//     if (effect)
//     {
//       maskEffect = effect;
//
//       const auto info = canvas->imageInfo();
//       SkRuntimeShaderBuilder builder(effect);
//       builder.uniform("rect") = SkV4{ (float)info.width(), (float)info.height(), 0, 0 };
//       builder.child("input_1") = objectShader;
//       builder.child("input_2") = maskShader;
//       SkPaint maskPaint;
//       auto shader = builder.makeShader(nullptr, false);
//       maskPaint.setShader(shader);
//       // canvas->drawPaint(maskPaint);
//     }
//     else
//     {
//       WARN("%s", err.c_str());
//     }
//   }
// }
PathNode::PathNode(const std::string& name)
  : PaintNode(name, ObjectType::VGG_PATH)
{
}

Mask PathNode::asOutlineMask(const glm::mat3* mat)
{
  SkPath p;
  Mask mask;
  if (!shape.subshape.contours.empty())
  {
    for (const auto& c : shape.subshape.contours)
    {
      p.addPath(getSkiaPath(c, c.closed));
    }
  }
  if (mat)
  {
    p.transform(toSkMatrix(*mat));
  }
  mask.outlineMask = p;
  return mask;
}

void PathNode::Paint(SkCanvas* canvas)
{
  if (!shape.subshape.contours.empty())
  {
    auto mask = makeMaskBy(BO_Intersection);
    if (mask.outlineMask.isEmpty())
    {
      drawContour(canvas, nullptr, nullptr);
    }
    else
    {
      drawContour(canvas, nullptr, &mask.outlineMask);
    }
  }
} // namespace VGG

void PathNode::drawContour(SkCanvas* canvas, sk_sp<SkShader> shader, const SkPath* outlineMask)
{
  SkPath skPath;
  for (const auto& contour : shape.subshape.contours)
  {
    skPath.addPath(getSkiaPath(contour, contour.closed));
  }

  canvas->save();
  canvas->scale(1, -1);

  if (outlineMask)
  {
    SkPaint maskPaint;
    maskPaint.setColor(SkColors::kRed);
    maskPaint.setStyle(SkPaint::kFill_Style);
    canvas->drawPath(*outlineMask, maskPaint);
  }

  // winding rule
  if (shape.windingRule == EWindingType::WR_EvenOdd)
  {
    skPath.setFillType(SkPathFillType::kEvenOdd);
  }
  else
  {
    skPath.setFillType(SkPathFillType::kWinding);
  }

  const auto globalAlpha = contextSetting.Opacity;

  for (const auto& f : style.fills)
  {
    if (!f.isEnabled)
      continue;
    SkPaint fillPen;
    if (shader)
    {
      fillPen.setShader(shader);
    }
    fillPen.setColor(f.color);
    fillPen.setStyle(SkPaint::kFill_Style);
    fillPen.setAlphaf(fillPen.getAlphaf() * globalAlpha);
    if (outlineMask)
    {
      canvas->clipPath(*outlineMask);
    }
    canvas->drawPath(skPath, fillPen);
  }

  // draw boarders
  //

  SkPaint strokePen;
  strokePen.setAntiAlias(true);
  strokePen.setStyle(SkPaint::kStroke_Style);
  for (const auto& b : style.borders)
  {
    if (!b.is_enabled)
      continue;
    if (shader)
    {
      strokePen.setShader(shader);
    }
    strokePen.setStrokeWidth(b.thickness);
    strokePen.setColor(b.color.value_or(VGGColor{ .r = 0, .g = 0, .b = 0, .a = 1.0 }));

    if (outlineMask)
    {
      canvas->clipPath(*outlineMask);
    }
    canvas->drawPath(skPath, strokePen);
  }

  canvas->restore();
}

} // namespace VGG
