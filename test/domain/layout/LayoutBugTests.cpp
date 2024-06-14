#include "base.hpp"

#include "Domain/Layout/Layout.hpp"
#include "Domain/Model/Element.hpp"
#include "UseCase/StartRunning.hpp"

#include "domain/model/daruma_helper.hpp"

#include "test_config.hpp"

#include <gtest/gtest.h>

namespace VGG
{
namespace
{

class LayoutBugTestSuite : public BaseVggLayoutTestSuite
{
protected:
  void layout(Layout::Size size)
  {
    layoutAllPage(size, true);
  }
};

TEST_F(LayoutBugTestSuite, AbsoluteContainer)
{
  // Given
  setupWithExpanding("testDataDir/layout/bugs/24_0524_01/");

  // When
  layout(Layout::Size{ 876, 900 });

  // Then
  {
    auto         node = m_sut->layoutTree()->findDescendantNodeById("7:45109"); // d101, Cantainer
    Layout::Rect expectedFrame = { { 183, 8 }, { 511, 48 } };
    EXPECT_EQ(node->frame(), expectedFrame);
  }
}

TEST_F(LayoutBugTestSuite, NestedHugHeightContainer)
{
  // Given
  setupWithExpanding("testDataDir/layout/bugs/24_0524_01/");

  // When
  layout(Layout::Size{ 876, 900 });

  // Then
  {
    auto         node = m_sut->layoutTree()->findDescendantNodeById("7:47085"); // d1, Section
    Layout::Rect expectedFrame = { { 0, 0 }, { 876, 440 } };
    EXPECT_EQ(node->frame(), expectedFrame);
  }
}

TEST_F(LayoutBugTestSuite, ChangeContainerHugWidthToFixed) // child width is fill
{
  // Given
  setupWithExpanding("testDataDir/layout/bugs/24_0510_02/");

  // When
  layout(Layout::Size{ 876, 900 });

  // Then
  {
    auto node =
      m_sut->layoutTree()->findDescendantNodeById("1:204"); // "name": "Home Screen Quick Actions"
    Layout::Rect expectedFrame = { { 31, 196 }, { 250, 264 } };
    EXPECT_EQ(node->frame(), expectedFrame);
  }
}

TEST_F(LayoutBugTestSuite, PresentState)
{
  // Given
  setupWithExpanding("testDataDir/layout/bugs/24_0607_05/");

  // When
  layout(Layout::Size{ 1920, 900 });

  // Then
  {
    auto node =
      m_sut->layoutTree()->findDescendantNodeById("1:124__1:59"); // "name": "d11, image 1"
    Layout::Rect expectedFrame = { { 0, 0 }, { 1856, 217 } };
    EXPECT_EQ(node->frame(), expectedFrame);
  }
}

} // namespace
} // namespace VGG