// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "BaseIntegrationTest.h"
#include "CustomFeeLimit.h"
#include "ED25519PrivateKey.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicInfo.h"
#include "TopicInfoQuery.h"
#include "TopicMessageSubmitTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace Hiero;

class TopicMessageSubmitTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline const std::string& getTestMemo() const { return mMemo; }
  [[nodiscard]] inline const std::string& getTestBigContents() const { return mBigContents; }

private:
  const std::string mMemo = "[e2e::TopicCreateTransaction]";
  const std::string mBigContents =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur aliquam augue sem, ut mattis dui laoreet a. "
    "Curabitur consequat est euismod, scelerisque metus et, tristique dui. Nulla commodo mauris ut faucibus ultricies. "
    "Quisque venenatis nisl nec augue tempus, at efficitur elit eleifend. Duis pharetra felis metus, sed dapibus urna "
    "vehicula id. Duis non venenatis turpis, sit amet ornare orci. Donec non interdum quam. Sed finibus nunc et risus "
    "finibus, non sagittis lorem cursus. Proin pellentesque tempor aliquam. Sed congue nisl in enim bibendum, "
    "condimentum vehicula nisi feugiat.\n"
    "\n"
    "Suspendisse non sodales arcu. Suspendisse sodales, lorem ac mollis blandit, ipsum neque porttitor nulla, et "
    "sodales arcu ante fermentum tellus. Integer sagittis dolor sed augue fringilla accumsan. Cras vitae finibus arcu, "
    "sit amet varius dolor. Etiam id finibus dolor, vitae luctus velit. Proin efficitur augue nec pharetra accumsan. "
    "Aliquam lobortis nisl diam, vel fermentum purus finibus id. Etiam at finibus orci, et tincidunt turpis. Aliquam "
    "imperdiet congue lacus vel facilisis. Phasellus id magna vitae enim dapibus vestibulum vitae quis augue. Morbi eu "
    "consequat enim. Maecenas neque nulla, pulvinar sit amet consequat sed, tempor sed magna. Mauris lacinia sem "
    "feugiat faucibus aliquet. Etiam congue non turpis at commodo. Nulla facilisi.\n"
    "\n"
    "Nunc velit turpis, cursus ornare fringilla eu, lacinia posuere leo. Mauris rutrum ultricies dui et suscipit. "
    "Curabitur in euismod ligula. Curabitur vitae faucibus orci. Phasellus volutpat vestibulum diam sit amet "
    "vestibulum. In vel purus leo. Nulla condimentum lectus vestibulum lectus faucibus, id lobortis eros consequat. "
    "Proin mollis libero elit, vel aliquet nisi imperdiet et. Morbi ornare est velit, in vehicula nunc malesuada quis. "
    "Donec vehicula convallis interdum.\n"
    "\n"
    "Integer pellentesque in nibh vitae aliquet. Ut at justo id libero dignissim hendrerit. Interdum et malesuada "
    "fames ac ante ipsum primis in faucibus. Praesent quis ornare lectus. Nam malesuada non diam quis cursus. "
    "Phasellus a libero ligula. Suspendisse ligula elit, congue et nisi sit amet, cursus euismod dolor. Morbi aliquam, "
    "nulla a posuere pellentesque, diam massa ornare risus, nec eleifend neque eros et elit.\n"
    "\n"
    "Pellentesque a sodales dui. Sed in efficitur ante. Duis eget volutpat elit, et ornare est. Nam felis dolor, "
    "placerat mattis diam id, maximus lobortis quam. Sed pellentesque lobortis sem sed placerat. Pellentesque augue "
    "odio, molestie sed lectus sit amet, congue volutpat massa. Quisque congue consequat nunc id fringilla. Duis "
    "semper nulla eget enim venenatis dapibus. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per "
    "inceptos himenaeos. Pellentesque varius turpis nibh, sit amet malesuada mauris malesuada quis. Vivamus dictum "
    "egestas placerat. Vivamus id augue elit.\n"
    "\n"
    "Cras fermentum volutpat eros, vitae euismod lorem viverra nec. Donec lectus augue, porta eleifend odio sit amet, "
    "condimentum rhoncus urna. Nunc sed odio velit. Morbi non cursus odio, eget imperdiet erat. Nunc rhoncus massa non "
    "neque volutpat, sit amet faucibus ante congue. Phasellus nec lorem vel leo accumsan lobortis. Maecenas id ligula "
    "bibendum purus suscipit posuere ac eget diam. Nam in quam pretium, semper erat auctor, iaculis odio. Maecenas "
    "placerat, nisi ac elementum tempor, felis nulla porttitor orci, ac rhoncus diam justo in elit. Etiam lobortis "
    "fermentum ligula in tincidunt. Donec quis vestibulum nunc. Sed eros diam, interdum in porta lobortis, gravida eu "
    "magna. Donec diam purus, finibus et sollicitudin quis, fringilla nec nisi. Pellentesque habitant morbi tristique "
    "senectus et netus et malesuada fames ac turpis egestas. Curabitur ultricies sagittis dapibus. Etiam ullamcorper "
    "aliquet libero, eu venenatis mauris suscipit id.\n"
    "\n"
    "Ut interdum eleifend sem, vel bibendum ipsum volutpat eget. Nunc ac dignissim augue. Aliquam ornare aliquet magna "
    "id dignissim. Vestibulum velit sem, lacinia eu rutrum in, rhoncus vitae mauris. Pellentesque scelerisque pulvinar "
    "mauris non cursus. Integer id dolor porta, bibendum sem vel, pretium tortor. Fusce a nisi convallis, interdum "
    "quam condimentum, suscipit dolor. Sed magna diam, efficitur non nunc in, tincidunt varius mi. Aliquam ullamcorper "
    "nulla eu fermentum bibendum. Vivamus a felis pretium, hendrerit enim vitae, hendrerit leo. Suspendisse lacinia "
    "lectus a diam consectetur suscipit. Aenean hendrerit nisl sed diam venenatis pellentesque. Nullam egestas lectus "
    "a consequat pharetra. Vivamus ornare tellus auctor, facilisis lacus id, feugiat dui. Nam id est ut est rhoncus "
    "varius.\n"
    "\n"
    "Aenean vel vehicula erat. Aenean gravida risus vitae purus sodales, quis dictum enim porta. Ut elit elit, "
    "fermentum sed posuere non, accumsan eu justo. Integer porta malesuada quam, et elementum massa suscipit nec. "
    "Donec in elit diam. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. "
    "Duis suscipit vel ante volutpat vestibulum.\n"
    "\n"
    "Pellentesque ex arcu, euismod et sapien ut, vulputate suscipit enim. Donec mattis sagittis augue, et mattis "
    "lacus. Cras placerat consequat lorem sed luctus. Nam suscipit aliquam sem ac imperdiet. Mauris a semper augue, "
    "pulvinar fringilla magna. Integer pretium massa non risus commodo hendrerit. Sed dictum libero id erat sodales "
    "mattis. Etiam auctor dolor lectus, ut sagittis enim lobortis vitae. Orci varius natoque penatibus et magnis dis "
    "parturient montes, nascetur ridiculus mus. Curabitur nec orci lobortis, cursus risus eget, sollicitudin massa. "
    "Integer vel tincidunt mi, id eleifend quam. Nullam facilisis nisl eu mauris congue, vitae euismod nisi malesuada. "
    "Vivamus sit amet urna et libero sagittis dictum.\n"
    "\n"
    "In hac habitasse platea dictumst. Aliquam erat volutpat. Ut dictum, mi a viverra venenatis, mi urna pulvinar "
    "nisi, nec gravida lectus diam eget urna. Ut dictum sit amet nisl ut dignissim. Sed sed mauris scelerisque, "
    "efficitur augue in, vulputate turpis. Proin dolor justo, bibendum et sollicitudin feugiat, imperdiet sed mi. Sed "
    "elementum vitae massa vel lobortis. Cras vitae massa sit amet libero dictum tempus.\n"
    "\n"
    "Vivamus ut mauris lectus. Curabitur placerat ornare scelerisque. Cras malesuada nunc quis tortor pretium bibendum "
    "vel sed dui. Cras lobortis nibh eu erat blandit, sit amet consequat neque fermentum. Phasellus imperdiet molestie "
    "tristique. Cras auctor purus erat, id mollis ligula porttitor eget. Mauris porta pharetra odio et finibus. "
    "Suspendisse eu est a ligula bibendum cursus. Mauris ac laoreet libero. Nullam volutpat sem quis rhoncus "
    "gravida.\n"
    "\n"
    "Donec malesuada lacus ac iaculis cursus. Sed sem orci, feugiat ac est ut, ultricies posuere nisi. Suspendisse "
    "potenti. Phasellus ut ultricies purus. Etiam sem tortor, fermentum quis aliquam eget, consequat ut nulla. Aliquam "
    "dictum metus in mi fringilla, vel gravida nulla accumsan. Cras aliquam eget leo vel posuere. Vivamus vitae "
    "malesuada nunc. Morbi placerat magna mi, id suscipit lacus auctor quis. Nam at lorem vel odio finibus fringilla "
    "ut ac velit. Donec laoreet at nibh quis vehicula.\n"
    "\n"
    "Fusce ac tristique nisi. Donec leo nisi, consectetur at tellus sit amet, consectetur ultrices dui. Quisque "
    "gravida mollis tempor. Maecenas semper, sapien ut dignissim feugiat, massa enim viverra dolor, non varius eros "
    "nulla nec felis. Nunc massa lacus, ornare et feugiat id, bibendum quis purus. Praesent viverra elit sit amet "
    "purus consectetur venenatis. Maecenas nibh risus, elementum sit amet enim sagittis, ultrices malesuada lectus. "
    "Vivamus non felis ante. Ut vulputate ex arcu. Aliquam porta gravida porta. Aliquam eros leo, malesuada quis eros "
    "non, maximus tristique neque. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus "
    "mus. Etiam ligula orci, mollis vel luctus nec, venenatis vitae est. Fusce rutrum convallis nisi.\n"
    "\n"
    "Nunc laoreet eget nibh in feugiat. Pellentesque nec arcu cursus, gravida dolor a, pellentesque nisi. Praesent vel "
    "justo blandit, placerat risus eget, consectetur orci. Sed maximus metus mi, ut euismod augue ultricies in. Nunc "
    "id risus hendrerit, aliquet lorem nec, congue justo. Vestibulum vel nunc ac est euismod mattis ac vitae nulla. "
    "Donec blandit luctus mauris, sit amet bibendum dui egestas et. Aenean nec lorem nec elit ornare rutrum nec eget "
    "ligula. Fusce a ipsum vitae neque elementum pharetra. Pellentesque ullamcorper ullamcorper libero, vitae porta "
    "sem sagittis vel. Interdum et malesuada fames ac ante ipsum primis in faucibus.\n"
    "\n"
    "Duis at massa sit amet risus pellentesque varius sit amet vitae eros. Cras tempor aliquet sapien, vehicula varius "
    "neque volutpat et. Donec purus nibh, pellentesque ut lobortis nec, ultricies ultricies nisl. Sed accumsan ut dui "
    "sit amet vulputate. Suspendisse eu facilisis massa, a hendrerit mauris. Nulla elementum molestie tincidunt. Donec "
    "mi justo, ornare vel tempor id, gravida et orci. Nam molestie erat nec nisi bibendum accumsan. Duis vitae tempor "
    "ante. Morbi congue mauris vel sagittis facilisis. Vivamus vehicula odio orci, a tempor nibh euismod in. Proin "
    "mattis, nibh at feugiat porta, purus velit posuere felis, quis volutpat sapien dui vel odio. Nam fermentum sem "
    "nec euismod aliquet. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis "
    "egestas. Aliquam erat volutpat.\n"
    "\n"
    "Mauris congue lacus tortor. Pellentesque arcu eros, accumsan imperdiet porttitor vitae, mattis nec justo. Nullam "
    "ac aliquam mauris. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. "
    "Suspendisse potenti. Fusce accumsan tempus felis a sagittis. Maecenas et velit odio. Vestibulum ante ipsum primis "
    "in faucibus orci luctus et ultrices posuere cubilia curae; Aliquam eros lacus, volutpat non urna sed, tincidunt "
    "ullamcorper elit. Sed sit amet gravida libero. In varius mi vel diam sollicitudin mollis.\n"
    "\n"
    "Aenean varius, diam vitae hendrerit feugiat, libero augue ultrices odio, eget consequat sem tellus eu nisi. Nam "
    "dapibus enim et auctor sollicitudin. Nunc iaculis eros orci, ac accumsan eros malesuada ut. Ut semper augue "
    "felis, nec sodales lorem consectetur non. Cras gravida eleifend est, et sagittis eros imperdiet congue. Fusce id "
    "tellus dapibus nunc scelerisque tempus. Donec tempor placerat libero, in commodo nisi bibendum eu. Donec id eros "
    "non est sollicitudin luctus. Duis bibendum bibendum tellus nec viverra. Aliquam non faucibus ex, nec luctus dui. "
    "Curabitur efficitur varius urna non dignissim. Suspendisse elit elit, ultrices in elementum eu, tempor at "
    "magna.\n"
    "\n"
    "Nunc in purus sit amet mi laoreet pulvinar placerat eget sapien. Donec vel felis at dui ultricies euismod quis "
    "vel neque. Donec tincidunt urna non eros pretium blandit. Nullam congue tincidunt condimentum. Curabitur et "
    "libero nibh. Proin ultricies risus id imperdiet scelerisque. Suspendisse purus mi, viverra vitae risus ut, tempus "
    "tincidunt enim. Ut luctus lobortis nisl, eget venenatis tortor cursus non. Mauris finibus nisl quis gravida "
    "ultricies. Fusce elementum lacus sit amet nunc congue, in porta nulla tincidunt.\n"
    "\n"
    "Mauris ante risus, imperdiet blandit posuere in, blandit eu ipsum. Integer et auctor arcu. Integer quis elementum "
    "purus. Nunc in ultricies nisl, sed rutrum risus. Suspendisse venenatis eros nec lorem rhoncus, in scelerisque "
    "velit condimentum. Etiam condimentum quam felis, in elementum odio mattis et. In ut nibh porttitor, hendrerit "
    "tellus vel, luctus magna. Vestibulum et ligula et dolor pellentesque porta. Aenean efficitur porta massa et "
    "bibendum. Nulla vehicula sem in risus volutpat, a eleifend elit maximus.\n"
    "\n"
    "Proin orci lorem, auctor a felis eu, pretium lobortis nulla. Phasellus aliquam efficitur interdum. Sed sit amet "
    "velit rutrum est dictum facilisis. Duis cursus enim at nisl tincidunt, eu molestie elit vehicula. Cras "
    "pellentesque nisl id enim feugiat fringilla. In quis tincidunt neque. Nam eu consectetur dolor. Ut id interdum "
    "mauris. Mauris nunc tortor, placerat in tempor ut, vestibulum eu nisl. Integer vel dapibus ipsum. Nunc id erat "
    "pulvinar, tincidunt magna id, condimentum massa. Pellentesque consequat est eget odio placerat vehicula. Etiam "
    "augue neque, sagittis non leo eu, tristique scelerisque dui. Ut dui urna, blandit quis urna ac, tincidunt "
    "tristique turpis.\n"
    "\n"
    "Suspendisse venenatis rhoncus ligula ultrices condimentum. In id laoreet eros. Suspendisse suscipit fringilla leo "
    "id euismod. Sed in quam libero. Ut at ligula tellus. Sed tristique gravida dui, at egestas odio aliquam iaculis. "
    "Praesent imperdiet velit quis nibh consequat, quis pretium sem sagittis. Donec tortor ex, congue sit amet "
    "pulvinar ac, rutrum non est. Praesent ipsum magna, venenatis sit amet vulputate id, eleifend ac ipsum.\n"
    "\n"
    "In consequat, nisi iaculis laoreet elementum, massa mauris varius nisi, et porta nisi velit at urna. Maecenas sit "
    "amet aliquet eros, a rhoncus nisl. Maecenas convallis enim nunc. Morbi purus nisl, aliquam ac tincidunt sed, "
    "mattis in augue. Quisque et elementum quam, vitae maximus orci. Suspendisse hendrerit risus nec vehicula "
    "placerat. Nulla et lectus nunc. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac "
    "turpis egestas.\n"
    "\n"
    "Etiam ut sodales ex. Nulla luctus, magna eu scelerisque sagittis, nibh quam consectetur neque, non rutrum dolor "
    "metus nec ex. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed "
    "egestas augue elit, sollicitudin accumsan massa lobortis ac. Curabitur placerat, dolor a aliquam maximus, velit "
    "ipsum laoreet ligula, id ullamcorper lacus nibh eget nisl. Donec eget lacus venenatis enim consequat auctor vel "
    "in.\n";
};

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, ExecuteTopicMessageSubmitTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  std::vector<TransactionResponse> txResponses;
  EXPECT_NO_THROW(
    txResponses =
      TopicMessageSubmitTransaction().setTopicId(topicId).setMessage("Hello from HCS!").executeAll(getTestClient()));

  // Then
  for (const auto& resp : txResponses)
  {
    EXPECT_EQ(resp.getReceipt(getTestClient()).mStatus, Status::SUCCESS);
  }

  TopicInfo topicInfo;
  ASSERT_NO_THROW(topicInfo = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  EXPECT_EQ(topicInfo.mTopicId, topicId);
  EXPECT_EQ(topicInfo.mSequenceNumber, 1);

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, ExecuteLargeTopicMessageSubmitTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  const std::string testMemo = getTestMemo();
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .setMemo(testMemo)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  TopicInfo topicInfo;
  EXPECT_NO_THROW(topicInfo = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  ASSERT_EQ(topicInfo.mTopicId, topicId);
  ASSERT_EQ(topicInfo.mMemo, testMemo);
  ASSERT_EQ(topicInfo.mSequenceNumber, 0);
  ASSERT_NE(topicInfo.mAdminKey, nullptr);
  ASSERT_EQ(topicInfo.mAdminKey->toBytes(), operatorKey->getPublicKey()->toBytes());

  // When
  std::vector<TransactionResponse> txResponses;
  EXPECT_NO_THROW(
    txResponses =
      TopicMessageSubmitTransaction().setTopicId(topicId).setMessage(getTestBigContents()).executeAll(getTestClient()));

  // Then
  for (const auto& resp : txResponses)
  {
    EXPECT_EQ(resp.getReceipt(getTestClient()).mStatus, Status::SUCCESS);
  }

  TopicInfo topicInfo2;
  ASSERT_NO_THROW(topicInfo2 = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  EXPECT_EQ(topicInfo2.mTopicId, topicId);
  EXPECT_EQ(topicInfo2.mMemo, testMemo);
  EXPECT_EQ(topicInfo2.mSequenceNumber, 14);
  ASSERT_NE(topicInfo2.mAdminKey, nullptr);
  EXPECT_EQ(topicInfo2.mAdminKey->toBytes(), operatorKey->getPublicKey()->toBytes());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, CannotSubmitTopicMessageWithoutTopicId)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  const std::string testMemo = getTestMemo();
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .setMemo(testMemo)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  std::vector<TransactionResponse> txResponses;
  ASSERT_THROW(
    txResponses =
      TopicMessageSubmitTransaction().setMessage(getTestBigContents()).setMaxChunks(15).executeAll(getTestClient()),
    PrecheckStatusException);

  // Then
  for (const auto& resp : txResponses)
  {
    EXPECT_EQ(resp.getValidateStatus(), true);
    EXPECT_THROW(const TransactionReceipt txReceipt = resp.getReceipt(getTestClient()), ReceiptStatusException);
  }

  TopicInfo topicInfo2;
  ASSERT_NO_THROW(topicInfo2 = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  EXPECT_EQ(topicInfo2.mTopicId, topicId);
  EXPECT_EQ(topicInfo2.mMemo, testMemo);
  EXPECT_EQ(topicInfo2.mSequenceNumber, 0);
  ASSERT_NE(topicInfo2.mAdminKey, nullptr);
  EXPECT_EQ(topicInfo2.mAdminKey->toBytes(), operatorKey->getPublicKey()->toBytes());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, CanSubmitTopicMessageWithoutMessage)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  const std::string testMemo = getTestMemo();
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .setMemo(testMemo)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  std::vector<TransactionResponse> txResponses;
  EXPECT_NO_THROW(txResponses = TopicMessageSubmitTransaction().setTopicId(topicId).executeAll(getTestClient()));

  // Then
  for (const auto& resp : txResponses)
  {
    EXPECT_EQ(resp.getReceipt(getTestClient()).mStatus, Status::SUCCESS);
  }

  TopicInfo topicInfo2;
  ASSERT_NO_THROW(topicInfo2 = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  EXPECT_EQ(topicInfo2.mTopicId, topicId);
  EXPECT_EQ(topicInfo2.mMemo, testMemo);
  ASSERT_NE(topicInfo2.mAdminKey, nullptr);
  EXPECT_EQ(topicInfo2.mAdminKey->toBytes(), operatorKey->getPublicKey()->toBytes());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCanChargeHbarsWithLimit)
{
  // Given
  int64_t feeAmount = 100000000; // 1 HBAR equivalent

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee = CustomFixedFee().setAmount(feeAmount).setFeeCollectorAccountId(
                    getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  Hbar initialBalance = Hbar(3LL);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setInitialBalance(initialBalance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  CustomFeeLimit limit;
  ASSERT_NO_THROW(limit = CustomFeeLimit().setPayerId(accountId).addCustomFee(CustomFixedFee().setAmount(feeAmount)));

  // When
  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .addCustomFeeLimit(limit)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountInfo accountInfo;
  EXPECT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_LT(accountInfo.mBalance.toTinybars(), initialBalance.toTinybars() - feeAmount);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCanChargeHbarsWithoutLimit)
{
  // Given
  int64_t feeAmount = 100000000; // 1 HBAR equivalent

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee = CustomFixedFee().setAmount(feeAmount).setFeeCollectorAccountId(
                    getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  Hbar initialBalance = Hbar(3LL);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setInitialBalance(initialBalance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountInfo accountInfo;
  EXPECT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_LT(accountInfo.mBalance.toTinybars(), initialBalance.toTinybars() - feeAmount);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCanChargeTokensWithLimit)
{
  // Given
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  uint64_t accountTokenBalance = 1;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(Hbar(2LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -accountTokenBalance)
                    .addTokenTransfer(tokenId, accountId, accountTokenBalance)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  CustomFeeLimit limit;
  EXPECT_NO_THROW(limit = CustomFeeLimit().setPayerId(accountId).addCustomFee(
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId)));

  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .addCustomFeeLimit(limit)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountBalance accountBalance;
  EXPECT_NO_THROW(accountBalance = AccountBalanceQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_LT(accountBalance.mTokens[tokenId], accountTokenBalance);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCanChargeTokensWithoutLimit)
{
  // Given
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  uint64_t accountTokenBalance = 1;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(Hbar(2LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -accountTokenBalance)
                    .addTokenTransfer(tokenId, accountId, accountTokenBalance)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountBalance accountBalance;
  EXPECT_NO_THROW(accountBalance = AccountBalanceQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_LT(accountBalance.mTokens[tokenId], accountTokenBalance);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicDoesNotChargeHbarsFeeExemptKeys)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  Hbar initialBalance = Hbar(3LL);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setInitialBalance(initialBalance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  int64_t feeAmount = 100000000; // 1 HBAR equivalent

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee = CustomFixedFee().setAmount(feeAmount).setFeeCollectorAccountId(
                    getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addFeeExemptKey(accountKey)
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountInfo accountInfo;
  EXPECT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_LT(initialBalance.toTinybars() - accountInfo.mBalance.toTinybars(), feeAmount);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicDoesNotChargeTokensFeeExemptKeys)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  Hbar initialBalance = Hbar(3LL);
  uint64_t accountTokenBalance = 1;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(initialBalance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addFeeExemptKey(accountKey)
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -accountTokenBalance)
                    .addTokenTransfer(tokenId, accountId, accountTokenBalance)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountBalance accountBalance;
  EXPECT_NO_THROW(accountBalance = AccountBalanceQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountBalance.mTokens[tokenId], accountTokenBalance);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCannotChargeHbarsWithLowerLimit)
{
  // Given
  int64_t feeAmount = 100000000; // 1 HBAR equivalent

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee = CustomFixedFee().setAmount(feeAmount).setFeeCollectorAccountId(
                    getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  Hbar initialBalance = Hbar(3LL);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setInitialBalance(initialBalance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  CustomFeeLimit limit;
  ASSERT_NO_THROW(limit =
                    CustomFeeLimit().setPayerId(accountId).addCustomFee(CustomFixedFee().setAmount(feeAmount - 1)));

  // When / Then
  // Submit a message to the revenue generating topic with lower fee limit than the actual fee required
  setTestClientOperator(accountId, accountKey);
  EXPECT_THROW(TopicMessageSubmitTransaction()
                 .setTopicId(topicId)
                 .setMessage("message")
                 .addCustomFeeLimit(limit)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // MAX_CUSTOM_FEE_LIMIT_EXCEEDED
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCannotChargeTokensWithLowerLimit)
{
  // Given
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(2).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  uint64_t accountTokenBalance = 1;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(Hbar(2LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -accountTokenBalance)
                    .addTokenTransfer(tokenId, accountId, accountTokenBalance)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  CustomFeeLimit limit;
  EXPECT_NO_THROW(limit = CustomFeeLimit().setPayerId(accountId).addCustomFee(
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId)));

  setTestClientOperator(accountId, accountKey);
  EXPECT_THROW(TopicMessageSubmitTransaction()
                 .setTopicId(topicId)
                 .addCustomFeeLimit(limit)
                 .setMessage("message")
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // MAX_CUSTOM_FEE_LIMIT_EXCEEDED
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCannotChargeTokensWithInvalidCustomFeeLimit)
{
  // Given
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  uint64_t accountTokenBalance = 1;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(Hbar(2LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -accountTokenBalance)
                    .addTokenTransfer(tokenId, accountId, accountTokenBalance)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  CustomFeeLimit limit;
  EXPECT_NO_THROW(limit = CustomFeeLimit().setPayerId(accountId).addCustomFee(
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(TokenId(0, 0, 0))));

  setTestClientOperator(accountId, accountKey);
  EXPECT_THROW(TopicMessageSubmitTransaction()
                 .setTopicId(topicId)
                 .addCustomFeeLimit(limit)
                 .setMessage("message")
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // NO_VALID_MAX_CUSTOM_FEE
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicDoesNotChargeTreasuries)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  uint64_t accountTokenBalance = 10;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(Hbar(2LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(accountId)
                              .freezeWith(&getTestClient())
                              .sign(accountKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(getTestClient().getOperatorAccountId().value())
                    .setTokenIds({ tokenId })
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, accountId, -1ULL)
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), 1ULL)
                    .freezeWith(&getTestClient())
                    .sign(accountKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountBalance accountBalance;
  EXPECT_NO_THROW(accountBalance = AccountBalanceQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountBalance.mTokens[tokenId], accountTokenBalance - 1); // -1 as 1 was sent to the operator account
}