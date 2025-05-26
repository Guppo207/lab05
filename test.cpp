#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Account.h>
#include <Transaction.h>

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    
    MOCK_METHOD(void, Unlock, (), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(int, id, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(int, GetBalance, (), (override));
};

class MockTransaction : public Transaction {
public:
    MOCK_METHOD(bool, Make, (Account& from, Account& to, int sum), (override));
    MOCK_METHOD(void, set_fee, (int fee), (override));
    MOCK_METHOD(int, fee, (), (override));
};

TEST(AccountTest, BalanceAndIdOperations) {
    MockAccount account(1, 100);
    
    EXPECT_CALL(account, GetBalance()).Times(3);
    EXPECT_CALL(account, Lock()).Times(1);
    EXPECT_CALL(account, Unlock()).Times(1);
    EXPECT_CALL(account, ChangeBalance(testing::_)).Times(2);
    EXPECT_CALL(account, id()).Times(1);
    
    account.GetBalance();
    account.id();
    account.Unlock();
    account.ChangeBalance(1000);
    account.GetBalance();
    account.ChangeBalance(2);
    account.GetBalance();
    account.Lock();
}

TEST(AccountTest, LockingMechanism) {
    Account account(0, 100);
    
    EXPECT_THROW(account.ChangeBalance(50), std::runtime_error);
    
    account.Lock();
    account.ChangeBalance(50);
    EXPECT_EQ(account.GetBalance(), 150);
    
    EXPECT_THROW(account.Lock(), std::runtime_error);
    account.Unlock();
}

TEST(TransactionTest, TransferOperations) {
    MockTransaction transaction;
    MockAccount first_account(1, 100);
    MockAccount second_account(2, 250);
    MockAccount flat_org(3, 10000);
    MockAccount organization(4, 5000);
    
    EXPECT_CALL(transaction, set_fee(testing::_)).Times(1);
    EXPECT_CALL(transaction, fee()).Times(1);
    EXPECT_CALL(transaction, Make(testing::_, testing::_, testing::_)).Times(2);
    EXPECT_CALL(first_account, GetBalance()).Times(1);
    EXPECT_CALL(second_account, GetBalance()).Times(1);
    
    transaction.set_fee(300);
    transaction.Make(first_account, second_account, 2000);
    transaction.fee();
    first_account.GetBalance();
    second_account.GetBalance();
    transaction.Make(organization, first_account, 1000);
}
