#pragma once
#include "Layer/Core/VType.hpp"
#include "Layer/Core/Attrs.hpp"
#include <modules/skparagraph/include/DartTypes.h>
#include <modules/skparagraph/include/Metrics.h>
#include <modules/skparagraph/include/Paragraph.h>
#include <modules/skparagraph/include/ParagraphBuilder.h>
#include <modules/skparagraph/include/ParagraphStyle.h>
#include <modules/skparagraph/include/FontCollection.h>
#include <modules/skparagraph/include/TextStyle.h>
#include <modules/skparagraph/include/TypefaceFontProvider.h>

#include "Layer/Core/Node.hpp"

namespace VGG
{

using namespace skia::textlayout;

struct TextView
{
  std::string_view Text;
  size_t Count;
  TextView() = default;
  TextView(const std::string_view& text, size_t count)
    : Text(text)
    , Count(count)
  {
  }
};
class TextParagraph
{
public:
  std::unique_ptr<ParagraphBuilder> Builder{ nullptr };
  int Level{ 0 };
  TextView Utf8TextView;
  TextParagraph() = default;
  TextParagraph(std::string_view view,
                std::unique_ptr<ParagraphBuilder> builder,
                int level,
                size_t charCount)
    : Builder(std::move(builder))
    , Level(level)
    , Utf8TextView({ view, charCount })
  {
  }
};

struct ParagraphAttr
{
  TextLineAttr type;
  ETextHorizontalAlignment horiAlign;
  ParagraphAttr() = default;
  ParagraphAttr(TextLineAttr type, ETextHorizontalAlignment align)
    : type(type)
    , horiAlign(align)
  {
  }

  ParagraphAttr(const ParagraphAttr&) = default;
  ParagraphAttr& operator=(const ParagraphAttr&) = default;

  ParagraphAttr(ParagraphAttr&&) = default;
  ParagraphAttr& operator=(ParagraphAttr&&) = default;
};

class ParagraphListener
{
  friend class ParagraphParser;

public:
  ParagraphListener() = default;
  ParagraphListener(const ParagraphListener&) = default;
  ParagraphListener& operator=(const ParagraphListener&) = default;

  ParagraphListener(ParagraphListener&&) noexcept = default;
  ParagraphListener& operator=(ParagraphListener&&) noexcept = default;

protected:
  virtual void onBegin() = 0;
  virtual void onEnd() = 0;
  virtual void onParagraphBegin(int paraIndex, int order, const ParagraphAttr& paragraAttr) = 0;
  virtual void onParagraphEnd(int paraIndex, const TextView& textView) = 0;
  virtual void onTextStyle(int paraIndex,
                           int styleIndex,
                           const TextView& textView,
                           const TextAttr& textAttr) = 0;
};

class ParagraphParser
{
  int m_length{ 0 };
  int m_styleIndex{ 0 };
  int m_paragraphAttrIndex{ 0 };
  const char* m_prevStyleBegin{ nullptr };
  const char* m_prevParagraphBegin{ nullptr };
  int m_offset{ 0 };
  bool m_seperateLines{ false };
  struct LevelOrderState
  {
    std::unordered_map<int, int> level2Order;
    void reset()
    {
      level2Order.clear();
    }
    int order(int currentLevel, int isFirstLine)
    {
      int currentOrder = 0;
      if (!isFirstLine)
      {
        currentOrder = level2Order[currentLevel] + 1;
        level2Order[currentLevel] = currentOrder;
      }
      else
      {
        // reset to 0 for the first line
        level2Order[currentLevel] = currentOrder;
      }
      return currentOrder;
    }
  } m_orderState;
  void reset(const std::string& text, int firstOffset)
  {
    m_styleIndex = 0;
    m_paragraphAttrIndex = 0;
    m_offset = firstOffset;
    m_length = 0;
    m_prevStyleBegin = text.c_str();
    m_prevParagraphBegin = text.c_str();
    m_orderState.reset();
  }

public:
  ParagraphParser(bool seperateLines = false)
    : m_seperateLines(seperateLines)
  {
  }

  void parse(ParagraphListener& listener,
             const std::string& text,
             const std::vector<TextAttr>& textAttrs,
             const std::vector<ParagraphAttr>& paragraphAttributes);
};
} // namespace VGG