/*
 * Copyright 2023-2024 VeryGoodGraphics LTD <bd@verygoodgraphics.com>
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

#include "UseCase/InstanceState.hpp"

#include "Domain/Layout/Node.hpp"
#include "Domain/Model/Element.hpp"
#include "Utility/Log.hpp"

using namespace VGG;

InstanceState::InstanceState(std::shared_ptr<LayoutNode> page)
  : m_page{ page }
{
  ASSERT(page);
}

bool InstanceState::presentState(
  const std::string& instanceDescendantId,
  const std::string& stateMasterId,
  StateTree*         stateTree)
{
  ASSERT(stateTree);
  auto instanceNode = findInstanceNode(instanceDescendantId);
  if (!instanceNode)
  {
    return false;
  }

  // save current state tree to still handle events
  auto pInstance = static_cast<Domain::SymbolInstanceElement*>(instanceNode->elementNode().get());
  ASSERT(pInstance);
  auto oldMasterId = pInstance->masterId();
  auto oldElementChildren = pInstance->presentState(stateMasterId);
  auto oldLayoutChildren = instanceNode->removeAllChildren();

  auto treeElement = std::make_shared<Domain::StateTreeElement>(instanceNode->elementNode());
  for (auto& child : oldElementChildren)
  {
    treeElement->addChild(child);
  }

  stateTree->setSrcNode(instanceNode);
  stateTree->setTreeElement(treeElement);
  for (auto& child : oldLayoutChildren)
  {
    stateTree->addChild(child);
  }

  return setMasterId(instanceDescendantId, stateMasterId);
}

bool InstanceState::dismissState(const std::string& instanceDescendantId)
{
  auto instanceNode = findInstanceNode(instanceDescendantId);
  if (!instanceNode)
  {
    return false;
  }

  auto pInstance = static_cast<Domain::SymbolInstanceElement*>(instanceNode->elementNode().get());
  ASSERT(pInstance);
  auto oldMasterId = pInstance->dissmissState();

  return setMasterId(instanceDescendantId, oldMasterId);
}

bool InstanceState::setMasterId(
  const std::string& instanceDescendantId,
  const std::string& stateMasterId)
{
  // todo
  auto instanceNode = findInstanceNode(instanceDescendantId);
  if (!instanceNode)
  {
    return false;
  }

  auto pInstance = static_cast<Domain::SymbolInstanceElement*>(instanceNode->elementNode().get());
  ASSERT(pInstance);

  auto oldElementChildren = pInstance->updateMasterId(stateMasterId);
  instanceNode->removeAllChildren();

  // expand again
  // update view model
  // render

  return false;
}

std::shared_ptr<LayoutNode> InstanceState::findInstanceNode(const std::string& instanceDescendantId)
{
  // find instance, ignore master
  auto node = m_page->findDescendantNodeById(instanceDescendantId);
  if (!node)
  {
    return nullptr;
  }

  std::shared_ptr<LayoutNode> instanceNode;
  while (node)
  {
    if (node->elementNode()->type() == Domain::Element::EType::SYMBOL_INSTANCE)
    {
      // todo, check master id
      instanceNode = node;
      break;
    }
    node = node->parent();
  }
  return instanceNode;
}