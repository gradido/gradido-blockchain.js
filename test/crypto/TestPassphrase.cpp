#include "TestPassphrase.h"

#include "gradido_blockchain/crypto/Passphrase.h"

#include "sodium.h"

#include "gtest/gtest.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "gradido_blockchain/crypto/KeyPairEd25519.h"

using namespace magic_enum;

void PassphraseTest::SetUp()
{
	uint16_t wordIndices1[] = {
			157, 1629, 1619, 1100, 2032, 1185, 1767, 1787, 1325, 963, 1906, 680, 1127, 252, 1547, 853, 414, 1569, 1767, 990, 755, 1593, 431, 722};
	std::string passphrases1[] = {
			"Fichte Heuschrecke Botschafter G&ouml;ttingen Leasing losfliegen simpel enorm erk&auml;mpft Werk Wolke vorhanden jene Slums stagnieren Verifizieren insgesamt Hanau simpel Inspiration delegieren umtauschen ablegen suggerieren ",
			"ankommt gesamt gestorben m&uuml;de sind stolz Enkel geklappt Leonardo riesig Bezugsquelle berauben pr&uuml;fen bislang Villa Fortschritt moralisch unf&auml;hig Enkel erwidern Hanau Plage Fossilien ethnisch ",
			"beauty slight skill maze wrap neither table term pizza journey unusual fence mind buzz scrap height critic service table knock fury shrimp curious fog "};
	mPassphrasesForTesting.push_back(PassphraseDataSet(
			passphrases1,
			MnemonicType::BIP0039_SORTED_ORDER,
			"6fa7180b132e1248c649fc7b2e422ad57663299f85bd88b8b8031dce28b501a8",
			wordIndices1));

	uint16_t wordIndices2[] = {
			742, 837, 269, 1067, 310, 1169, 1272, 365, 44, 619, 719, 108, 1750, 464, 1607, 663, 815, 343, 1784, 85, 510, 1809, 66, 1559};
	std::string passphrases2[] = {
			"oftmals bist bietet spalten Datenbank Masse str&auml;flich hervor Derartig Hallo christlich Brief iPhone einpendeln telefonieren musizieren gigantisch Orchester zirkulieren essen gilt Erich Dollar money ",
			"Angst ausbeuten besser bekannt Bed&uuml;rfnisse Neidisch virtuell Pension gepr&auml;gt inmitten Abfall Absatzmarkt Wettbewerb Fidel jeder Heinrich Engagement leihen viertel Disziplin zufolge erwarten Iris J&auml;ger ",
			"fresh hamster canvas lyrics chat mutual pair color airport estate fly assault suspect deliver similar fancy grass cliff tenant apple divert timber analyst seed "};
	mPassphrasesForTesting.push_back(PassphraseDataSet(
			passphrases2,
			MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER,
			"8943813a623863dd7c5e5b248e408ac8a8851ef758275b6043a06e9b5832c36c",
			wordIndices2));

	uint16_t wordIndices3[] = {
			1412, 777, 1270, 905, 673, 670, 1780, 143, 546, 452, 505, 977, 1616, 916, 1635, 1318, 364, 1229, 1078, 1850, 514, 361, 990, 632};
	std::string passphrases3[] = {
			"tief Acker Abgaben jenseits Revolution verdeckt Entdeckung Sanktion sammeln Umdrehung regulieren murmeln Erkenntnis hart zwar zuspitzen indem fegen bomber zw&ouml;lf Mobbing divers Inspiration Krieg ",
			"aushalten absolut signifikant Bezahlung zukunftsf&auml;hig Wurzel ergr&uuml;nden unausweichlich dunkel halb Nagel nehmen Begabung bezwingen wehren Fohlen keiner Krankheit leiblich &Auml;ste Finnland sehen erwidern Abs&auml;tze ",
			"rack gentle paddle illness feature fatigue teach ball dust decade dish kick skate income small pill collect often man trap doctor coffee knock excuse "};
	mPassphrasesForTesting.push_back(PassphraseDataSet(
			passphrases3,
			MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER,
			"d62f14173ae5d66b06753cc9d69d5471913ffc6053feedac2acf901eef3582a9",
			wordIndices3));
	CryptoConfig::loadMnemonicWordLists(true);
}

TEST_F(PassphraseTest, detectMnemonic)
{
	for (auto it = mPassphrasesForTesting.begin(); it != mPassphrasesForTesting.end(); it++)
	{
		auto testDataSet = *it;
		for (int i = 0; i < enum_integer(MnemonicType::MAX); i++)
		{
			auto type = enum_cast<MnemonicType>(i).value();
			// currently not easy to differentiate from MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER
			if (MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER_FIXED_CASES == type)
				continue;
			EXPECT_EQ(Passphrase::detectMnemonic(testDataSet.passphrases[i]), &CryptoConfig::g_Mnemonic_WordLists[i]);
		}
	}
	EXPECT_FALSE(Passphrase::detectMnemonic("Dies ist eine ungültige Passphrase"));
}

TEST_F(PassphraseTest, detectMnemonicWithPubkey)
{
	auto pubkeyBin = std::make_shared<memory::Block>(crypto_sign_PUBLICKEYBYTES);
	for (auto it = mPassphrasesForTesting.begin(); it != mPassphrasesForTesting.end(); it++)
	{
		auto testDataSet = *it;
		// testDataSet.pubkeyHex

		EXPECT_NO_THROW(pubkeyBin->fromHex(testDataSet.pubkeyHex));

		auto key_pair = new KeyPairEd25519(pubkeyBin);
		for (int i = 0; i < enum_integer(MnemonicType::MAX); i++)
		{
			printf("try to detect mnemonic for passphrase: %s, i: %d\n", testDataSet.passphrases[i].data(), i);
			EXPECT_EQ(Passphrase::detectMnemonic(testDataSet.passphrases[i], key_pair), &CryptoConfig::g_Mnemonic_WordLists[i]);
		}
	}
}

TEST_F(PassphraseTest, filter)
{
	std::string unfilteredPassphrases[] = {
			"oftmals bist bietet spalten Datenbank Masse sträflich hervor Derartig Hallo christlich Brief iPhone einpendeln telefonieren musizieren gigantisch Orchester zirkulieren essen gilt Erich Dollar money",
			"Höh, maß, xDäöas"};
	std::string filteredPassphrases[] = {
			"oftmals bist bietet spalten Datenbank Masse str&auml;flich hervor Derartig Hallo christlich Brief iPhone einpendeln telefonieren musizieren gigantisch Orchester zirkulieren essen gilt Erich Dollar money",
			"H&ouml;h ma&szlig; xD&auml;&ouml;as"};

	for (int i = 0; i < 2; i++)
	{
		EXPECT_EQ(Passphrase::filter(unfilteredPassphrases[i]), filteredPassphrases[i]);
	}
}

TEST_F(PassphraseTest, constructAndValid)
{
	for (auto it = mPassphrasesForTesting.begin(); it != mPassphrasesForTesting.end(); it++)
	{
		auto testDataSet = *it;
		for (int i = 0; i < enum_integer(MnemonicType::MAX); i++)
		{
			Passphrase passphrase(testDataSet.passphrases[i], &CryptoConfig::g_Mnemonic_WordLists[i]);
			EXPECT_TRUE(passphrase.checkIfValid());
		}
	}
	// skip because it will stop tests even with try catch block
	/*try
	{
		Passphrase passphrase(
				"Höh, maß, xDäöas",
				&CryptoConfig::g_Mnemonic_WordLists[enum_integer(MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER)]);
	}
	catch (MnemonicException &ex)
	{
		SUCCEED();
	}*/
}

TEST_F(PassphraseTest, wordIndices)
{
	for (auto it = mPassphrasesForTesting.begin(); it != mPassphrasesForTesting.end(); it++)
	{
		auto testDataSet = *it;
		for (int i = 0; i < enum_integer(MnemonicType::MAX); i++)
		{
			Passphrase passphrase(testDataSet.passphrases[i], &CryptoConfig::g_Mnemonic_WordLists[i]);
			ASSERT_TRUE(passphrase.checkIfValid());
			auto wordIndices = passphrase.getWordIndices();
			EXPECT_TRUE(wordIndices == testDataSet.wordIndices);
		}
	}
}

TEST_F(PassphraseTest, createAndTransform)
{
	for (auto it = mPassphrasesForTesting.begin(); it != mPassphrasesForTesting.end(); it++)
	{
		auto test_data_set = *it;
		auto mnemonic = &CryptoConfig::g_Mnemonic_WordLists[enum_integer(test_data_set.mnemonicType)];
		auto tr = Passphrase::create(test_data_set.wordIndices, mnemonic);

		auto word_indices = tr->getWordIndices();

		for (int i = 0; i < PHRASE_WORD_COUNT; i++)
		{
			EXPECT_EQ(word_indices[i], test_data_set.wordIndices[i]);
		}
		auto key_pair_ed25519 = KeyPairEd25519::create(tr);
		// KeyPair key_pair;
		// key_pair.generateFromPassphrase(test_data_set.passphrases[test_data_set.mnemonicType].data(), mnemonic);
		EXPECT_EQ(key_pair_ed25519->getPublicKey()->convertToHex(), test_data_set.pubkeyHex);
		// EXPECT_EQ(key_pair.getPubkeyHex(), test_data_set.pubkeyHex);

		// auto key_pair_old
		ASSERT_FALSE(tr == nullptr);
		EXPECT_TRUE(tr->checkIfValid());
		auto tr_english = tr->transform(&CryptoConfig::g_Mnemonic_WordLists[enum_integer(MnemonicType::BIP0039_SORTED_ORDER)]);
		ASSERT_FALSE(tr_english == nullptr);
		EXPECT_EQ(tr_english->getString(), test_data_set.passphrases[enum_integer(MnemonicType::BIP0039_SORTED_ORDER)]);
		// printf("english: %s\n", tr_english->getString().data());

		auto tr_german1 = tr->transform(&CryptoConfig::g_Mnemonic_WordLists[enum_integer(MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER)]);
		ASSERT_FALSE(tr_german1 == nullptr);
		EXPECT_EQ(tr_german1->getString(), test_data_set.passphrases[enum_integer(MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER)]);
		// printf("german 1: %s\n", tr_german1->getString().data());

		auto tr_german2 = tr->transform(&CryptoConfig::g_Mnemonic_WordLists[enum_integer(MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER_FIXED_CASES)]);
		ASSERT_FALSE(tr_german2 == nullptr);
		EXPECT_EQ(tr_german2->getString(), test_data_set.passphrases[enum_integer(MnemonicType::GRADIDO_BOOK_GERMAN_RANDOM_ORDER_FIXED_CASES)]);
		// printf("german 2: %s\n\n", tr_german2->getString().data());
	}
}
