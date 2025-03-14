// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileAppendTransaction.h"
#include "FileContentsQuery.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "WrappedTransaction.h"
#include "impl/Utilities.h"
#include <string_view>

#include <gtest/gtest.h>

using namespace Hiero;

class FileAppendTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline std::string_view getContent() const { return mContent; }

private:
  std::string_view mContent =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur aliquam augue sem, ut mattis dui laoreet a. "
    "Curabitur consequat est euismod, scelerisque metus et, tristique dui. Nulla commodo mauris ut faucibus "
    "ultricies. Quisque venenatis nisl nec augue tempus, at efficitur elit eleifend. Duis pharetra felis metus, sed "
    "dapibus urna vehicula id. Duis non venenatis turpis, sit amet ornare orci. Donec non interdum quam. Sed finibus "
    "nunc et risus finibus, non sagittis lorem cursus. Proin pellentesque tempor aliquam. Sed congue nisl in enim "
    "bibendum, condimentum vehicula nisi feugiat.\n\n"
    "Suspendisse non sodales arcu. Suspendisse sodales, lorem ac mollis blandit, ipsum neque porttitor nulla, et "
    "sodales arcu ante fermentum tellus. Integer sagittis dolor sed augue fringilla accumsan. Cras vitae finibus "
    "arcu, sit amet varius dolor. Etiam id finibus dolor, vitae luctus velit. Proin efficitur augue nec pharetra "
    "accumsan. Aliquam lobortis nisl diam, vel fermentum purus finibus id. Etiam at finibus orci, et tincidunt "
    "turpis. Aliquam imperdiet congue lacus vel facilisis. Phasellus id magna vitae enim dapibus vestibulum vitae "
    "quis augue. Morbi eu consequat enim. Maecenas neque nulla, pulvinar sit amet consequat sed, tempor sed magna. "
    "Mauris lacinia sem feugiat faucibus aliquet. Etiam congue non turpis at commodo. Nulla facilisi.\n\n"
    "Nunc velit turpis, cursus ornare fringilla eu, lacinia posuere leo. Mauris rutrum ultricies dui et suscipit. "
    "Curabitur in euismod ligula. Curabitur vitae faucibus orci. Phasellus volutpat vestibulum diam sit amet "
    "vestibulum. In vel purus leo. Nulla condimentum lectus vestibulum lectus faucibus, id lobortis eros "
    "consequat. Proin mollis libero elit, vel aliquet nisi imperdiet et. Morbi ornare est velit, in vehicula nunc "
    "malesuada quis. Donec vehicula convallis interdum.\n\n"
    "Integer pellentesque in nibh vitae aliquet. Ut at justo id libero dignissim hendrerit. Interdum et malesuada "
    "fames ac ante ipsum primis in faucibus. Praesent quis ornare lectus. Nam malesuada non diam quis cursus. "
    "Phasellus a libero ligula. Suspendisse ligula elit, congue et nisi sit amet, cursus euismod dolor. Morbi "
    "aliquam, nulla a posuere pellentesque, diam massa ornare risus, nec eleifend neque eros et elit.\n\n"
    "Pellentesque a sodales dui. Sed in efficitur ante. Duis eget volutpat elit, et ornare est. Nam felis dolor, "
    "placerat mattis diam id, maximus lobortis quam. Sed pellentesque lobortis sem sed placerat. Pellentesque "
    "augue odio, molestie sed lectus sit amet, congue volutpat massa. Quisque congue consequat nunc id fringilla. "
    "Duis semper nulla eget enim venenatis dapibus. Class aptent taciti sociosqu ad litora torquent per conubia "
    "nostra, per inceptos himenaeos. Pellentesque varius turpis nibh, sit amet malesuada mauris malesuada quis. "
    "Vivamus dictum egestas placerat. Vivamus id augue elit.\n\n"
    "Cras fermentum volutpat eros, vitae euismod lorem viverra nec. Donec lectus augue, porta eleifend odio sit "
    "amet, condimentum rhoncus urna. Nunc sed odio velit. Morbi non cursus odio, eget imperdiet erat. Nunc rhoncus "
    "massa non neque volutpat, sit amet faucibus ante congue. Phasellus nec lorem vel leo accumsan lobortis. "
    "Maecenas id ligula bibendum purus suscipit posuere ac eget diam. Nam in quam pretium, semper erat auctor, "
    "iaculis odio. Maecenas placerat, nisi ac elementum tempor, felis nulla porttitor orci, ac rhoncus diam justo "
    "in elit. Etiam lobortis fermentum ligula in tincidunt. Donec quis vestibulum nunc. Sed eros diam, interdum in "
    "porta lobortis, gravida eu magna. Donec diam purus, finibus et sollicitudin quis, fringilla nec nisi. "
    "Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur "
    "ultricies sagittis dapibus. Etiam ullamcorper aliquet libero, eu venenatis mauris suscipit id.\n\n"
    "Ut interdum eleifend sem, vel bibendum ipsum volutpat eget. Nunc ac dignissim augue. Aliquam ornare aliquet "
    "magna id dignissim. Vestibulum velit sem, lacinia eu rutrum in, rhoncus vitae mauris. Pellentesque "
    "scelerisque pulvinar mauris non cursus. Integer id dolor porta, bibendum sem vel, pretium tortor. Fusce a "
    "nisi convallis, interdum quam condimentum, suscipit dolor. Sed magna diam, efficitur non nunc in, tincidunt "
    "varius mi. Aliquam ullamcorper nulla eu fermentum bibendum. Vivamus a felis pretium, hendrerit enim vitae, "
    "hendrerit leo. Suspendisse lacinia lectus a diam consectetur suscipit. Aenean hendrerit nisl sed diam "
    "venenatis pellentesque. Nullam egestas lectus a consequat pharetra. Vivamus ornare tellus auctor, facilisis "
    "lacus id, feugiat dui. Nam id est ut est rhoncus varius.\n\n"
    "Aenean vel vehicula erat. Aenean gravida risus vitae purus sodales, quis dictum enim porta. Ut elit elit, "
    "fermentum sed posuere non, accumsan eu justo. Integer porta malesuada quam, et elementum massa suscipit nec. "
    "Donec in elit diam. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis "
    "egestas. Duis suscipit vel ante volutpat vestibulum.\n\n"
    "Pellentesque ex arcu, euismod et sapien ut, vulputate suscipit enim. Donec mattis sagittis augue, et mattis "
    "lacus. Cras placerat consequat lorem sed luctus. Nam suscipit aliquam sem ac imperdiet. Mauris a semper "
    "augue, pulvinar fringilla magna. Integer pretium massa non risus commodo hendrerit. Sed dictum libero id erat "
    "sodales mattis. Etiam auctor dolor lectus, ut sagittis enim lobortis vitae. Orci varius natoque penatibus et "
    "magnis dis parturient montes, nascetur ridiculus mus. Curabitur nec orci lobortis, cursus risus eget, "
    "sollicitudin massa. Integer vel tincidunt mi, id eleifend quam. Nullam facilisis nisl eu mauris congue, vitae "
    "euismod nisi malesuada. Vivamus sit amet urna et libero sagittis dictum.\n\n"
    "In hac habitasse platea dictumst. Aliquam erat volutpat. Ut dictum, mi a viverra venenatis, mi urna pulvinar "
    "nisi, nec gravida lectus diam eget urna. Ut dictum sit amet nisl ut dignissim. Sed sed mauris scelerisque, "
    "efficitur augue in, vulputate turpis. Proin dolor justo, bibendum et sollicitudin feugiat, imperdiet sed mi. "
    "Sed elementum vitae massa vel lobortis. Cras vitae massa sit amet libero dictum tempus.\n\n"
    "Vivamus ut mauris lectus. Curabitur placerat ornare scelerisque. Cras malesuada nunc quis tortor pretium "
    "bibendum vel sed dui. Cras lobortis nibh eu erat blandit, sit amet consequat neque fermentum. Phasellus "
    "imperdiet molestie tristique. Cras auctor purus erat, id mollis ligula porttitor eget. Mauris porta pharetra "
    "odio et finibus. Suspendisse eu est a ligula bibendum cursus. Mauris ac laoreet libero. Nullam volutpat sem "
    "quis rhoncus gravida.\n\n"
    "Donec malesuada lacus ac iaculis cursus. Sed sem orci, feugiat ac est ut, ultricies posuere nisi. Suspendisse "
    "potenti. Phasellus ut ultricies purus. Etiam sem tortor, fermentum quis aliquam eget, consequat ut nulla. "
    "Aliquam dictum metus in mi fringilla, vel gravida nulla accumsan. Cras aliquam eget leo vel posuere. Vivamus "
    "vitae malesuada nunc. Morbi placerat magna mi, id suscipit lacus auctor quis. Nam at lorem vel odio finibus "
    "fringilla ut ac velit. Donec laoreet at nibh quis vehicula.\n\n"
    "Fusce ac tristique nisi. Donec leo nisi, consectetur at tellus sit amet, consectetur ultrices dui. Quisque "
    "gravida mollis tempor. Maecenas semper, sapien ut dignissim feugiat, massa enim viverra dolor, non varius "
    "eros nulla nec felis. Nunc massa lacus, ornare et feugiat id, bibendum quis purus. Praesent viverra elit sit "
    "amet purus consectetur venenatis. Maecenas nibh risus, elementum sit amet enim sagittis, ultrices malesuada "
    "lectus. Vivamus non felis ante. Ut vulputate ex arcu. Aliquam porta gravida porta. Aliquam eros leo, "
    "malesuada quis eros non, maximus tristique neque. Orci varius natoque penatibus et magnis dis parturient "
    "montes, nascetur ridiculus mus. Etiam ligula orci, mollis vel luctus nec, venenatis vitae est. Fusce rutrum "
    "convallis nisi.\n\n"
    "Nunc laoreet eget nibh in feugiat. Pellentesque nec arcu cursus, gravida dolor a, pellentesque nisi. Praesent "
    "vel justo blandit, placerat risus eget, consectetur orci. Sed maximus metus mi, ut euismod augue ultricies "
    "in. Nunc id risus hendrerit, aliquet lorem nec, congue justo. Vestibulum vel nunc ac est euismod mattis ac "
    "vitae nulla. Donec blandit luctus mauris, sit amet bibendum dui egestas et. Aenean nec lorem nec elit ornare "
    "rutrum nec eget ligula. Fusce a ipsum vitae neque elementum pharetra. Pellentesque ullamcorper ullamcorper "
    "libero, vitae porta sem sagittis vel. Interdum et malesuada fames ac ante ipsum primis in faucibus.\n\n"
    "Duis at massa sit amet risus pellentesque varius sit amet vitae eros. Cras tempor aliquet sapien, vehicula "
    "varius neque volutpat et. Donec purus nibh, pellentesque ut lobortis nec, ultricies ultricies nisl. Sed "
    "accumsan ut dui sit amet vulputate. Suspendisse eu facilisis massa, a hendrerit mauris. Nulla elementum "
    "molestie tincidunt. Donec mi justo, ornare vel tempor id, gravida et orci. Nam molestie erat nec nisi "
    "bibendum accumsan. Duis vitae tempor ante. Morbi congue mauris vel sagittis facilisis. Vivamus vehicula odio "
    "orci, a tempor nibh euismod in. Proin mattis, nibh at feugiat porta, purus velit posuere felis, quis volutpat "
    "sapien dui vel odio. Nam fermentum sem nec euismod aliquet. Pellentesque habitant morbi tristique senectus et "
    "netus et malesuada fames ac turpis egestas. Aliquam erat volutpat.\n\n"
    "Mauris congue lacus tortor. Pellentesque arcu eros, accumsan imperdiet porttitor vitae, mattis nec justo. "
    "Nullam ac aliquam mauris. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus "
    "mus. Suspendisse potenti. Fusce accumsan tempus felis a sagittis. Maecenas et velit odio. Vestibulum ante "
    "ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Aliquam eros lacus, volutpat non urna "
    "sed, tincidunt ullamcorper elit. Sed sit amet gravida libero. In varius mi vel diam sollicitudin mollis.\n\n"
    "Aenean varius, diam vitae hendrerit feugiat, libero augue ultrices odio, eget consequat sem tellus eu nisi. "
    "Nam dapibus enim et auctor sollicitudin. Nunc iaculis eros orci, ac accumsan eros malesuada ut. Ut semper "
    "augue felis, nec sodales lorem consectetur non. Cras gravida eleifend est, et sagittis eros imperdiet congue. "
    "Fusce id tellus dapibus nunc scelerisque tempus. Donec tempor placerat libero, in commodo nisi bibendum eu. "
    "Donec id eros non est sollicitudin luctus. Duis bibendum bibendum tellus nec viverra. Aliquam non faucibus "
    "ex, nec luctus dui. Curabitur efficitur varius urna non dignissim. Suspendisse elit elit, ultrices in "
    "elementum eu, tempor at magna.\n\n"
    "Nunc in purus sit amet mi laoreet pulvinar placerat eget sapien. Donec vel felis at dui ultricies euismod "
    "quis vel neque. Donec tincidunt urna non eros pretium blandit. Nullam congue tincidunt condimentum. Curabitur "
    "et libero nibh. Proin ultricies risus id imperdiet scelerisque. Suspendisse purus mi, viverra vitae risus ut, "
    "tempus tincidunt enim. Ut luctus lobortis nisl, eget venenatis tortor cursus non. Mauris finibus nisl quis "
    "gravida ultricies. Fusce elementum lacus sit amet nunc congue, in porta nulla tincidunt.\n\n"
    "Mauris ante risus, imperdiet blandit posuere in, blandit eu ipsum. Integer et auctor arcu. Integer quis "
    "elementum purus. Nunc in ultricies nisl, sed rutrum risus. Suspendisse venenatis eros nec lorem rhoncus, in "
    "scelerisque velit condimentum. Etiam condimentum quam felis, in elementum odio mattis et. In ut nibh "
    "porttitor, hendrerit tellus vel, luctus magna. Vestibulum et ligula et dolor pellentesque porta. Aenean "
    "efficitur porta massa et bibendum. Nulla vehicula sem in risus volutpat, a eleifend elit maximus.\n\n"
    "Proin orci lorem, auctor a felis eu, pretium lobortis nulla. Phasellus aliquam efficitur interdum. Sed sit "
    "amet velit rutrum est dictum facilisis. Duis cursus enim at nisl tincidunt, eu molestie elit vehicula. Cras "
    "pellentesque nisl id enim feugiat fringilla. In quis tincidunt neque. Nam eu consectetur dolor. Ut id "
    "interdum mauris. Mauris nunc tortor, placerat in tempor ut, vestibulum eu nisl. Integer vel dapibus ipsum. "
    "Nunc id erat pulvinar, tincidunt magna id, condimentum massa. Pellentesque consequat est eget odio placerat "
    "vehicula. Etiam augue neque, sagittis non leo eu, tristique scelerisque dui. Ut dui urna, blandit quis urna "
    "ac, tincidunt tristique turpis.\n\n"
    "Suspendisse venenatis rhoncus ligula ultrices condimentum. In id laoreet eros. Suspendisse suscipit fringilla "
    "leo id euismod. Sed in quam libero. Ut at ligula tellus. Sed tristique gravida dui, at egestas odio aliquam "
    "iaculis. Praesent imperdiet velit quis nibh consequat, quis pretium sem sagittis. Donec tortor ex, congue sit "
    "amet pulvinar ac, rutrum non est. Praesent ipsum magna, venenatis sit amet vulputate id, eleifend ac "
    "ipsum.\n\n"
    "In consequat, nisi iaculis laoreet elementum, massa mauris varius nisi, et porta nisi velit at urna. Maecenas "
    "sit amet aliquet eros, a rhoncus nisl. Maecenas convallis enim nunc. Morbi purus nisl, aliquam ac tincidunt "
    "sed, mattis in augue. Quisque et elementum quam, vitae maximus orci. Suspendisse hendrerit risus nec vehicula "
    "placerat. Nulla et lectus nunc. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac "
    "turpis egestas.\n\n"
    "Etiam ut sodales ex. Nulla luctus, magna eu scelerisque sagittis, nibh quam consectetur neque, non rutrum "
    "dolor metus nec ex. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos "
    "himenaeos. Sed egestas augue elit, sollicitudin accumsan massa lobortis ac. Curabitur placerat, dolor a "
    "aliquam maximus, velit ipsum laoreet ligula, id ullamcorper lacus nibh eget nisl. Donec eget lacus venenatis "
    "enim consequat auctor vel in.\n";
};

//-----
TEST_F(FileAppendTransactionIntegrationTests, ExecuteFileAppendTransaction)
{
  // Given
  std::unique_ptr<PrivateKey> operatorKey;
  std::vector<std::byte> origContents;
  std::vector<std::byte> appendedContents;
  FileId fileId;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(origContents = internal::Utilities::stringToByteVector("Hiero is great!"));
  ASSERT_NO_THROW(appendedContents = internal::Utilities::stringToByteVector(" It is also awesome!"));
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(origContents)
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = FileAppendTransaction()
                                                         .setFileId(fileId)
                                                         .setContents(appendedContents)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Then
  FileContents fileContents;
  ASSERT_NO_THROW(fileContents = FileContentsQuery().setFileId(fileId).execute(getTestClient()));
  EXPECT_EQ(fileContents, internal::Utilities::concatenateVectors({ origContents, appendedContents }));

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(FileAppendTransactionIntegrationTests, CanAppendLargeContents)
{
  // Given
  std::unique_ptr<PrivateKey> operatorKey;
  std::vector<std::byte> origContents;
  std::vector<std::byte> appendedContents;
  FileId fileId;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(origContents = internal::Utilities::stringToByteVector("Hiero is great! "));
  ASSERT_NO_THROW(appendedContents = internal::Utilities::stringToByteVector(getContent()));
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(origContents)
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  std::vector<TransactionResponse> txResponses;
  EXPECT_NO_THROW(
    txResponses = FileAppendTransaction().setFileId(fileId).setContents(appendedContents).executeAll(getTestClient()));

  // Then
  FileContents fileContents;
  ASSERT_NO_THROW(fileContents = FileContentsQuery().setFileId(fileId).execute(getTestClient()));
  EXPECT_EQ(fileContents, internal::Utilities::concatenateVectors({ origContents, appendedContents }));

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(FileAppendTransactionIntegrationTests, SerializeDeserializeExecute)
{
  // Given
  std::unique_ptr<PrivateKey> operatorKey;
  std::vector<std::byte> origContents;
  std::vector<std::byte> appendedContents;
  FileId fileId;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(origContents = internal::Utilities::stringToByteVector("Hiero is great! "));
  ASSERT_NO_THROW(appendedContents = internal::Utilities::stringToByteVector(getContent()));
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(origContents)
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  FileAppendTransaction fileAppend = FileAppendTransaction().setFileId(fileId).setContents(appendedContents);

  std::vector<std::byte> transactionBytes;
  ASSERT_NO_THROW(transactionBytes = fileAppend.toBytes(););

  WrappedTransaction wrappedTransaction;
  ASSERT_NO_THROW(wrappedTransaction = Transaction<FileAppendTransaction>::fromBytes(transactionBytes));

  fileAppend = *wrappedTransaction.getTransaction<FileAppendTransaction>();

  std::vector<TransactionResponse> txResponses;
  EXPECT_NO_THROW(txResponses = fileAppend.executeAll(getTestClient()));

  // Then
  FileContents fileContents;
  ASSERT_NO_THROW(fileContents = FileContentsQuery().setFileId(fileId).execute(getTestClient()));
  EXPECT_EQ(fileContents, internal::Utilities::concatenateVectors({ origContents, appendedContents }));

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(FileAppendTransactionIntegrationTests, FreezeSignSerializeDeserializeAndCompare)
{
  // Given
  const std::shared_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  std::vector<std::byte> origContents;
  std::vector<std::byte> appendedContents;
  FileId fileId;

  const TransactionType expectedType = TransactionType::FILE_APPEND_TRANSACTION;

  ASSERT_NO_THROW(origContents = internal::Utilities::stringToByteVector("Hiero is great! "));
  ASSERT_NO_THROW(appendedContents = internal::Utilities::stringToByteVector(getContent()));
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(origContents)
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  FileAppendTransaction fileAppend = FileAppendTransaction()
                                       .setFileId(fileId)
                                       .setContents(appendedContents)
                                       .freezeWith(&getTestClient())
                                       .sign(operatorKey);

  std::vector<std::byte> transactionBytes;
  ASSERT_NO_THROW(transactionBytes = fileAppend.toBytes(););

  WrappedTransaction wrappedTransaction;
  ASSERT_NO_THROW(wrappedTransaction = Transaction<FileAppendTransaction>::fromBytes(transactionBytes));

  fileAppend = *wrappedTransaction.getTransaction<FileAppendTransaction>();

  // Then
  std::vector<std::byte> reserializedTransactionBytes;
  ASSERT_NO_THROW(reserializedTransactionBytes = fileAppend.toBytes(););
  ASSERT_EQ(transactionBytes, reserializedTransactionBytes);
  EXPECT_EQ(expectedType, wrappedTransaction.getTransactionType());
}