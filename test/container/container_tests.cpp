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

#include "MockSkiaGraphicsContext.hpp"

#include "Entry/Container/Container.hpp"

#include "test_config.hpp"

#include <gtest/gtest.h>

#include <thread>

using namespace VGG;
class ContainerTestSuite : public ::testing::Test
{
protected:
  std::shared_ptr<Container> m_sut;

  void SetUp() override
  {
    m_sut.reset(new Container);
  }
};

TEST_F(ContainerTestSuite, Smoke)
{
  // Given

  // When

  // Then
}

TEST_F(ContainerTestSuite, LoadModel)
{
  // Given
  std::string filePath = "testDataDir/vgg-daruma-2";

  // When
  auto result = m_sut->load(filePath);

  // Then
  EXPECT_TRUE(result);
}

TEST_F(ContainerTestSuite, Render)
{
  // Given
  std::unique_ptr<layer::SkiaGraphicsContext> graphicsContext{ new MockSkiaGraphicsContext };
  m_sut->setGraphicsContext(graphicsContext, 600, 800);

  std::string filePath = "testDataDir/vgg-daruma-2";
  auto        result = m_sut->load(filePath);
  EXPECT_TRUE(result);

  // When
  {
    auto ret = m_sut->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // makeFrame
    ret = m_sut->run();
    EXPECT_TRUE(ret);
  }

  // Then
}

TEST_F(ContainerTestSuite, HandleEvent)
{
  // Given
  std::unique_ptr<layer::SkiaGraphicsContext> graphicsContext{ new MockSkiaGraphicsContext };
  m_sut->setGraphicsContext(graphicsContext, 600, 800);

  std::string filePath = "testDataDir/vgg-daruma-2";
  auto        result = m_sut->load(filePath);
  EXPECT_TRUE(result);

  bool called{ false };
  m_sut->setEventListener([&called](std::string type, std::string path) { called = true; });

  // When
  UEvent evt;
  evt.touch.type = VGG_TOUCHDOWN;
  evt.touch.windowX = 100;
  evt.touch.windowY = 100;

  m_sut->onEvent(evt);

  // Then
  EXPECT_TRUE(called);
}