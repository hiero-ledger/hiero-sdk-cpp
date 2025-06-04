// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_BASE_INTEGRATION_TEST_H_
#define HIERO_SDK_CPP_BASE_INTEGRATION_TEST_H_

#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"

#include <gtest/gtest.h>
#include <string>

namespace Hiero
{

class BaseIntegrationTest : public testing::Test
{
protected:
  /** Get the test Client used in integration tests.
   *
   * @return The test Client.
   */
  [[nodiscard]] inline const Client& getTestClient() const { return mClient; }

  /** Get the test file content used in integration tests.
   *
   * @return The test file content.
   */
  [[nodiscard]] inline const std::vector<std::byte>& getTestFileContent() const { return mFileContent; }

  /** Get tge test smart contract bytecode used in integration tests.
   *
   * @return The test smart contract bytecode in hex format.
   */
  [[nodiscard]] inline const std::string& getTestSmartContractBytecode() const { return mTestContractBytecodeHex; }

  /** Get the test big contents used in integration tests.
   *
   * @return The test big contents.
   */
  [[nodiscard]] inline const std::string& getTestBigContents() const { return mBigContents; }

  /** Set the test Client operator with the given account ID and private key.
   *
   * @param accountId The account ID of the operator.
   * @param privateKey The private key of the operator.
   */
  void setTestClientOperator(const AccountId& accountId, const std::shared_ptr<PrivateKey>& privateKey);

  /** Set the test Client operator with the default test account ID and private key.
   *
   * This method is used to set the operator for integration tests.
   */
  void setDefaultTestClientOperator();

  /** Set up the test environment.
   *
   * This method is called before each test case to initialize the test Client and file content.
   */
  void SetUp() override;

private:
  Client mClient;
  std::vector<std::byte> mFileContent;
  const AccountId mDefaultTestAccountId = AccountId::fromString("0.0.2");
  const std::shared_ptr<PrivateKey> mDefaultTestPrivateKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string mTestContractBytecodeHex =
    "608060405234801561001057600080fd5b506040516104d73803806104d78339818101604052602081101561003357600080fd5b8101908080"
    "51604051939291908464010000000082111561005357600080fd5b90830190602082018581111561006857600080fd5b825164010000000081"
    "118282018810171561008257600080fd5b82525081516020918201929091019080838360005b838110156100af578181015183820152602001"
    "610097565b50505050905090810190601f1680156100dc5780820380516001836020036101000a031916815260200191505b50604052505060"
    "0080546001600160a01b0319163317905550805161010890600190602084019061010f565b50506101aa565b82805460018160011615610100"
    "0203166002900490600052602060002090601f016020900481019282601f1061015057805160ff191683800117855561017d565b8280016001"
    "018555821561017d579182015b8281111561017d578251825591602001919060010190610162565b5061018992915061018d565b5090565b61"
    "01a791905b808211156101895760008155600101610193565b90565b61031e806101b96000396000f3fe608060405234801561001057600080"
    "fd5b50600436106100415760003560e01c8063368b87721461004657806341c0e1b5146100ee578063ce6d41de146100f6575b600080fd5b61"
    "00ec6004803603602081101561005c57600080fd5b81019060208101813564010000000081111561007757600080fd5b820183602082011115"
    "61008957600080fd5b803590602001918460018302840111640100000000831117156100ab57600080fd5b91908080601f0160208091040260"
    "20016040519081016040528093929190818152602001838380828437600092019190915250929550610173945050505050565b005b6100ec61"
    "01a2565b6100fe6101ba565b6040805160208082528351818301528351919283929083019185019080838360005b8381101561013857818101"
    "5183820152602001610120565b50505050905090810190601f1680156101655780820380516001836020036101000a03191681526020019150"
    "5b509250505060405180910390f35b6000546001600160a01b0316331461018a5761019f565b805161019d906001906020840190610250565b"
    "505b50565b6000546001600160a01b03163314156101b85733ff5b565b60018054604080516020601f60026000196101008789161502019095"
    "1694909404938401819004810282018101909252828152606093909290918301828280156102455780601f1061021a57610100808354040283"
    "529160200191610245565b820191906000526020600020905b81548152906001019060200180831161022857829003601f168201915b505050"
    "505090505b90565b828054600181600116156101000203166002900490600052602060002090601f016020900481019282601f106102915780"
    "5160ff19168380011785556102be565b828001600101855582156102be579182015b828111156102be57825182559160200191906001019061"
    "02a3565b506102ca9291506102ce565b5090565b61024d91905b808211156102ca57600081556001016102d456fea264697066735822122084"
    "964d4c3f6bc912a9d20e14e449721012d625aa3c8a12de41ae5519752fc89064736f6c63430006000033";
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

} // namespace Hiero

#endif // HIERO_SDK_CPP_BASE_INTEGRATION_TEST_H_