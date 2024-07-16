//
// Created by Anietta Weckauff on 07.07.24.
//
#include "../src/entities/Inventory.hpp"
#include "MockAudioService.h"
#include "MockResourceManager.h"
#include <gtest/gtest.h>

class InventoryTestFixture : public ::testing::Test {
  protected:
    std::unique_ptr<Inventory> inventory;

    void SetUp() override {
        MockResourceManager mockResourceManager(ConfigManager::getInstance());
        MockAudioService mockAudioService(mockResourceManager);
        ON_CALL(mockAudioService, playSound("pickup-item")).WillByDefault(::testing::Return());
        EXPECT_CALL(mockAudioService, playSound("pickup-item")).Times(1);
        inventory = std::make_unique<Inventory>(SLOTS_PER_INVENTORY, mockAudioService);
        Vector position = {0, 0};
        auto item = std::make_shared<Item>(DUCK_ITEM, position);
        inventory->addItem(item);
    }

    void TearDown() override {}
};

TEST_F(InventoryTestFixture, GetItemTypeTest) { EXPECT_EQ(inventory->getItemType(0), DUCK_ITEM); }

TEST_F(InventoryTestFixture, RemoveSelectedItemTest) {
    EXPECT_EQ(inventory->getItemType(0), DUCK_ITEM);
    inventory->removeSelectedItem();
    EXPECT_EQ(inventory->getItemType(0), NO_ITEM);
}

TEST_F(InventoryTestFixture, AddItemTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("pickup-item")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("pickup-item")).Times(3);
    inventory = std::make_unique<Inventory>(SLOTS_PER_INVENTORY, mockAudioService);
    Vector position0 = {0, 0};
    auto item0 = std::make_shared<Item>(DUCK_ITEM, position0);
    inventory->addItem(item0);

    Vector position = {0, 0};
    auto item = std::make_shared<Item>(KAISERSCHMARRN, position);
    inventory->addItem(item);
    int slotNumber = inventory->getSlotOfItem(KAISERSCHMARRN);
    EXPECT_EQ(inventory->getItemType(slotNumber), KAISERSCHMARRN);
    auto item2 = std::make_shared<Item>(DUCK_ITEM, position);
    inventory->addItem(item2);
    int slotNumber2 = inventory->getSlotOfItem(DUCK_ITEM);
    EXPECT_EQ(inventory->getItemType(slotNumber2), DUCK_ITEM);
}

TEST_F(InventoryTestFixture, GetSelectedItemTypeTest) { EXPECT_EQ(inventory->getSelectedItemType(), DUCK_ITEM); }

TEST_F(InventoryTestFixture, SwitchItemSlotTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("pickup-item")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("pickup-item")).Times(2);
    inventory = std::make_unique<Inventory>(SLOTS_PER_INVENTORY, mockAudioService);
    Vector position0 = {0, 0};
    auto item0 = std::make_shared<Item>(DUCK_ITEM, position0);
    inventory->addItem(item0);

    Vector position = {0, 0};
    auto item = std::make_shared<Item>(KAISERSCHMARRN, position);
    inventory->addItem(item);
    inventory->switchItemSlot(1);
    EXPECT_EQ(inventory->getSelectedItemType(), KAISERSCHMARRN);
}

TEST_F(InventoryTestFixture, SelectedSlotIsEmptyTest) {
    inventory->switchItemSlot(1);
    EXPECT_TRUE(inventory->selectedSlotIsEmpty());
}

TEST_F(InventoryTestFixture, GetSlotOfItemTest) { EXPECT_EQ(inventory->getSlotOfItem(DUCK_ITEM), 0); }

TEST_F(InventoryTestFixture, CanCollectItemTest) {
    Vector position = {0, 0};
    auto item = std::make_shared<Item>(KAISERSCHMARRN, position);
    EXPECT_TRUE(inventory->canCollectItem(item));
}

TEST_F(InventoryTestFixture, GetNumberOfItemsTest) {
    EXPECT_EQ(inventory->getNumberOfItems(0), 1);
    EXPECT_EQ(inventory->getNumberOfItems(1), 0);
    EXPECT_EQ(inventory->getNumberOfItems(2), 0);
}

TEST_F(InventoryTestFixture, SlotIsEmptyTest) {
    EXPECT_FALSE(inventory->slotIsEmpty(0));
    EXPECT_TRUE(inventory->slotIsEmpty(1));
    EXPECT_TRUE(inventory->slotIsEmpty(2));
}

TEST_F(InventoryTestFixture, GetItemTest) { EXPECT_EQ(inventory->getItem(0)->getItemType(), DUCK_ITEM); }
