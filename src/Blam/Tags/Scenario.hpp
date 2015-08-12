#pragma once
#include "Tags.hpp"
#include "../../Pointer.hpp"

namespace Blam
{
	namespace Tags
	{
		struct Scenario;

		inline Scenario *GetCurrentScenario()
		{
			auto scenario = Pointer::Base(0x1EAAEB4);
			return scenario.Read<Scenario*>();
		}

		struct Scenario : Tag<'scnr'>
		{
			struct TagBlock0;
			struct TagBlock1;
			struct TagBlock2;
			struct TagBlock17;
			struct TagBlock23;
			struct TagBlock24;
			struct TagBlock25;
			struct TagBlock26;
			struct TagBlock27;
			struct TagBlock28;
			struct TagBlock29;
			struct TagBlock30;
			struct TagBlock31;
			struct TagBlock32;
			struct TagBlock33;
			struct TagBlock34;
			struct TagBlock35;
			struct TagBlock36;
			struct TagBlock37;
			struct TagBlock38;
			struct TagBlock39;
			struct TagBlock40;
			struct TagBlock41;
			struct TagBlock42;
			struct TagBlock43;
			struct TagBlock44;
			struct TagBlock45;
			struct TagBlock46;
			struct TagBlock47;
			struct TagBlock48;
			struct TagBlock49;
			struct TagBlock50;
			struct PlayerStartingProfile;
			struct TagBlock52;
			struct TagBlock53;
			struct TagBlock55;
			struct TagBlock56;
			struct TagBlock57;
			struct TagBlock58;
			struct TagBlock61;
			struct TagBlock63;
			struct TagBlock64;
			struct TagBlock65;
			struct TagBlock66;
			struct TagBlock67;
			struct TagBlock70;
			struct TagBlock71;
			struct TagBlock72;
			struct TagBlock73;
			struct TagBlock74;
			struct TagBlock75;
			struct TagBlock76;
			struct TagBlock77;
			struct TagBlock85;
			struct TagBlock88;
			struct TagBlock92;
			struct TagBlock93;
			struct TagBlock94;
			struct TagBlock97;
			struct TagBlock98;
			struct TagBlock99;
			struct TagBlock100;
			struct TagBlock101;
			struct TagBlock102;

			int Unknown0;
			int Unknown4;
			int Unknown8;
			int UnknownC;
			int Unknown10;
			TagBlock<TagBlock0> Unknown14;
			int Unknown20;
			int Unknown24;
			int Unknown28;
			int Unknown2C;
			TagBlock<TagBlock1> Unknown30;
			TagBlock<TagBlock2> Unknown3C;
			TagBlock<TagBlock17> Unknown48;
			TagBlock<TagBlock23> Unknown54;
			int Unknown60;
			int Unknown64;
			int Unknown68;
			int Unknown6C;
			int Unknown70;
			int Unknown74;
			int Unknown78;
			int Unknown7C;
			int Unknown80;
			int Unknown84;
			int Unknown88;
			int Unknown8C;
			int Unknown90;
			int Unknown94;
			int Unknown98;
			int Unknown9C;
			int UnknownA0;
			int UnknownA4;
			int UnknownA8;
			int UnknownAC;
			int UnknownB0;
			int UnknownB4;
			int UnknownB8;
			TagBlock<TagBlock24> UnknownBC;
			TagBlock<TagBlock25> UnknownC8;
			TagBlock<TagBlock26> UnknownD4;
			TagBlock<TagBlock27> UnknownE0;
			TagBlock<TagBlock28> UnknownEC;
			TagBlock<TagBlock29> UnknownF8;
			TagBlock<TagBlock30> Unknown104;
			TagBlock<TagBlock31> Unknown110;
			TagBlock<TagBlock32> Unknown11C;
			TagBlock<TagBlock33> Unknown128;
			TagBlock<TagBlock34> Unknown134;
			int Unknown140;
			int Unknown144;
			int Unknown148;
			TagBlock<TagBlock35> Unknown14C;
			TagBlock<TagBlock36> Unknown158;
			int Unknown164;
			int Unknown168;
			int Unknown16C;
			int Unknown170;
			int Unknown174;
			int Unknown178;
			int Unknown17C;
			int Unknown180;
			int Unknown184;
			int Unknown188;
			int Unknown18C;
			int Unknown190;
			int Unknown194;
			int Unknown198;
			int Unknown19C;
			int Unknown1A0;
			int Unknown1A4;
			int Unknown1A8;
			TagBlock<TagBlock37> Unknown1AC;
			TagBlock<TagBlock38> Unknown1B8;
			int Unknown1C4;
			int Unknown1C8;
			int Unknown1CC;
			int Unknown1D0;
			int Unknown1D4;
			int Unknown1D8;
			TagBlock<TagBlock39> Unknown1DC;
			TagBlock<TagBlock40> Unknown1E8;
			TagBlock<TagBlock41> Unknown1F4;
			TagBlock<TagBlock42> Unknown200;
			TagBlock<TagBlock43> Unknown20C;
			TagBlock<TagBlock44> Unknown218;
			TagBlock<TagBlock45> Unknown224;
			TagBlock<TagBlock46> Unknown230;
			TagBlock<TagBlock47> Unknown23C;
			TagBlock<TagBlock48> Unknown248;
			TagBlock<TagBlock49> Unknown254;
			TagBlock<TagBlock50> Unknown260;
			TagBlock<PlayerStartingProfile> StartingProfile;
			TagBlock<TagBlock52> Unknown278;
			TagBlock<TagBlock53> Unknown284;
			int Unknown290;
			int Unknown294;
			int Unknown298;
			int Unknown29C;
			int Unknown2A0;
			int Unknown2A4;
			int Unknown2A8;
			int Unknown2AC;
			int Unknown2B0;
			int Unknown2B4;
			int Unknown2B8;
			int Unknown2BC;
			int Unknown2C0;
			int Unknown2C4;
			int Unknown2C8;
			int Unknown2CC;
			int Unknown2D0;
			int Unknown2D4;
			TagBlock<TagBlock55> Unknown2D8;
			int Unknown2E4;
			int Unknown2E8;
			int Unknown2EC;
			int Unknown2F0;
			int Unknown2F4;
			int Unknown2F8;
			int Unknown2FC;
			int Unknown300;
			int Unknown304;
			int Unknown308;
			int Unknown30C;
			int Unknown310;
			int Unknown314;
			int Unknown318;
			int Unknown31C;
			int Unknown320;
			int Unknown324;
			int Unknown328;
			int Unknown32C;
			int Unknown330;
			int Unknown334;
			int Unknown338;
			int Unknown33C;
			int Unknown340;
			int Unknown344;
			int Unknown348;
			int Unknown34C;
			int Unknown350;
			int Unknown354;
			int Unknown358;
			int Unknown35C;
			int Unknown360;
			int Unknown364;
			int Unknown368;
			int Unknown36C;
			int Unknown370;
			TagBlock<TagBlock56> Unknown374;
			TagBlock<TagBlock57> Unknown380;
			int Unknown38C;
			int Unknown390;
			int Unknown394;
			int Unknown398;
			int Unknown39C;
			int Unknown3A0;
			int Unknown3A4;
			int Unknown3A8;
			int Unknown3AC;
			int Unknown3B0;
			int Unknown3B4;
			int Unknown3B8;
			int Unknown3BC;
			int Unknown3C0;
			int Unknown3C4;
			int Unknown3C8;
			int Unknown3CC;
			int Unknown3D0;
			int Unknown3D4;
			int Unknown3D8;
			int Unknown3DC;
			int Unknown3E0;
			int Unknown3E4;
			int Unknown3E8;
			int Unknown3EC;
			int Unknown3F0;
			int Unknown3F4;
			TagBlock<TagBlock58> Unknown3F8;
			int Unknown404;
			int Unknown408;
			int Unknown40C;
			DataReference<uint8_t> Unknown410;
			TagBlock<TagBlock61> Unknown424;
			TagBlock<TagBlock63> Unknown430;
			TagBlock<TagBlock64> Unknown43C;
			int Unknown448;
			int Unknown44C;
			int Unknown450;
			TagBlock<TagBlock65> Unknown454;
			int Unknown460;
			int Unknown464;
			int Unknown468;
			TagBlock<TagBlock66> Unknown46C;
			int Unknown478;
			int Unknown47C;
			int Unknown480;
			int Unknown484;
			int Unknown488;
			int Unknown48C;
			int Unknown490;
			int Unknown494;
			int Unknown498;
			int Unknown49C;
			int Unknown4A0;
			TagBlock<TagBlock67> Unknown4A4;
			int Unknown4B0;
			int Unknown4B4;
			int Unknown4B8;
			TagBlock<TagBlock70> Unknown4BC;
			TagBlock<TagBlock71> Unknown4C8;
			TagBlock<TagBlock72> Unknown4D4;
			int Unknown4E0;
			int Unknown4E4;
			int Unknown4E8;
			int Unknown4EC;
			int Unknown4F0;
			int Unknown4F4;
			TagBlock<TagBlock73> Unknown4F8;
			int Unknown504;
			int Unknown508;
			int Unknown50C;
			int Unknown510;
			int Unknown514;
			int Unknown518;
			TagBlock<TagBlock74> Unknown51C;
			TagBlock<TagBlock75> Unknown528;
			TagBlock<TagBlock76> Unknown534;
			int Unknown540;
			int Unknown544;
			int Unknown548;
			int Unknown54C;
			int Unknown550;
			int Unknown554;
			int Unknown558;
			int Unknown55C;
			int Unknown560;
			TagBlock<TagBlock77> Unknown564;
			int Unknown570;
			int Unknown574;
			int Unknown578;
			int Unknown57C;
			int Unknown580;
			int Unknown584;
			int Unknown588;
			int Unknown58C;
			int Unknown590;
			int Unknown594;
			int Unknown598;
			int Unknown59C;
			int Unknown5A0;
			int Unknown5A4;
			int Unknown5A8;
			int Unknown5AC;
			int Unknown5B0;
			int Unknown5B4;
			int Unknown5B8;
			int Unknown5BC;
			int Unknown5C0;
			int Unknown5C4;
			int Unknown5C8;
			int Unknown5CC;
			int Unknown5D0;
			int Unknown5D4;
			int Unknown5D8;
			int Unknown5DC;
			int Unknown5E0;
			int Unknown5E4;
			int Unknown5E8;
			int Unknown5EC;
			int Unknown5F0;
			int Unknown5F4;
			int Unknown5F8;
			TagBlock<TagBlock85> Unknown5FC;
			int Unknown608;
			int Unknown60C;
			int Unknown610;
			int Unknown614;
			TagBlock<TagBlock88> Unknown618;
			TagBlock<TagBlock92> Unknown624;
			TagBlock<TagBlock93> Unknown630;
			TagBlock<TagBlock94> Unknown63C;
			int Unknown648;
			int Unknown64C;
			int Unknown650;
			int Unknown654;
			int Unknown658;
			int Unknown65C;
			int Unknown660;
			TagBlock<TagBlock97> Unknown664;
			TagBlock<TagBlock98> Unknown670;
			TagReference Unknown67C;
			int Unknown68C;
			int Unknown690;
			int Unknown694;
			int Unknown698;
			int Unknown69C;
			int Unknown6A0;
			int Unknown6A4;
			int Unknown6A8;
			int Unknown6AC;
			int Unknown6B0;
			int Unknown6B4;
			int Unknown6B8;
			int Unknown6BC;
			int Unknown6C0;
			int Unknown6C4;
			int Unknown6C8;
			int Unknown6CC;
			int Unknown6D0;
			int Unknown6D4;
			int Unknown6D8;
			int Unknown6DC;
			TagReference Unknown6E0;
			int Unknown6F0;
			int Unknown6F4;
			int Unknown6F8;
			int Unknown6FC;
			int Unknown700;
			int Unknown704;
			int Unknown708;
			int Unknown70C;
			TagReference Unknown710;
			TagReference Unknown720;
			TagReference Unknown730;
			TagReference Unknown740;
			int Unknown750;
			int Unknown754;
			int Unknown758;
			int Unknown75C;
			int Unknown760;
			int Unknown764;
			int Unknown768;
			int Unknown76C;
			int Unknown770;
			TagBlock<TagBlock99> Unknown774;
			int Unknown780;
			int Unknown784;
			int Unknown788;
			int Unknown78C;
			int Unknown790;
			int Unknown794;
			int Unknown798;
			int Unknown79C;
			int Unknown7A0;
			int Unknown7A4;
			int Unknown7A8;
			int Unknown7AC;
			int Unknown7B0;
			int Unknown7B4;
			int Unknown7B8;
			TagBlock<TagBlock100> Unknown7BC;
			TagBlock<TagBlock101> Unknown7C8;
			int Unknown7D4;
			int Unknown7D8;
			int Unknown7DC;
			int Unknown7E0;
			int Unknown7E4;
			int Unknown7E8;
			int Unknown7EC;
			int Unknown7F0;
			int Unknown7F4;
			int Unknown7F8;
			int Unknown7FC;
			int Unknown800;
			int Unknown804;
			int Unknown808;
			int Unknown80C;
			int Unknown810;
			int Unknown814;
			TagBlock<TagBlock102> Unknown818;

			struct TagBlock0
			{
				TagReference Unknown0;
				TagReference Unknown10;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				TagReference Unknown48;
				TagReference Unknown58;
				int Unknown68;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock0, 0x6C);

			struct TagBlock1
			{
				TagReference Unknown0;
				int Unknown10;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock1, 0x14);

			struct TagBlock2
			{
				struct TagBlock3;
				struct TagBlock4;
				struct TagBlock16;

				int Unknown0;
				int Unknown4;
				TagBlock<TagBlock3> Unknown8;
				TagBlock<TagBlock4> Unknown14;
				TagBlock<TagBlock16> Unknown20;

				struct TagBlock3
				{
					int Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock3, 0x4);

				struct TagBlock4
				{
					struct TagBlock5;
					struct TagBlock8;
					struct TagBlock11;
					struct TagBlock12;
					struct TagBlock13;
					struct TagBlock14;
					struct TagBlock15;

					TagBlock<TagBlock5> Unknown0;
					TagBlock<TagBlock8> UnknownC;
					TagBlock<TagBlock11> Unknown18;
					TagBlock<TagBlock12> Unknown24;
					TagBlock<TagBlock13> Unknown30;
					TagBlock<TagBlock14> Unknown3C;
					TagBlock<TagBlock15> Unknown48;

					struct TagBlock5
					{
						struct TagBlock6;

						TagBlock<TagBlock6> Unknown0;

						struct TagBlock6
						{
							struct TagBlock7;

							TagBlock<TagBlock7> Unknown0;

							struct TagBlock7
							{
								int Unknown0;
							};
							TAG_STRUCT_SIZE_ASSERT(TagBlock7, 0x4);
						};
						TAG_STRUCT_SIZE_ASSERT(TagBlock6, 0xC);
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock5, 0xC);

					struct TagBlock8
					{
						struct TagBlock9;

						TagBlock<TagBlock9> Unknown0;

						struct TagBlock9
						{
							struct TagBlock10;

							TagBlock<TagBlock10> Unknown0;

							struct TagBlock10
							{
								int Unknown0;
							};
							TAG_STRUCT_SIZE_ASSERT(TagBlock10, 0x4);
						};
						TAG_STRUCT_SIZE_ASSERT(TagBlock9, 0xC);
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock8, 0xC);

					struct TagBlock11
					{
						uint8_t Unknown0;
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock11, 0x1);

					struct TagBlock12
					{
						uint8_t Unknown0;
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock12, 0x1);

					struct TagBlock13
					{
						int Unknown0;
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock13, 0x4);

					struct TagBlock14
					{
						int Unknown0;
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock14, 0x4);

					struct TagBlock15
					{
						int Unknown0;
						int Unknown4;
						int Unknown8;
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock15, 0xC);
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock4, 0x54);

				struct TagBlock16
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
					int UnknownC;
					int Unknown10;
					int Unknown14;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock16, 0x18);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock2, 0x2C);

			struct TagBlock17
			{
				struct TagBlock18;
				struct TagBlock19;
				struct TagBlock20;
				struct TagBlock21;
				struct TagBlock22;

				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				TagBlock<TagBlock18> Unknown28;
				TagBlock<TagBlock19> Unknown34;
				TagBlock<TagBlock20> Unknown40;
				TagBlock<TagBlock21> Unknown4C;
				TagBlock<TagBlock22> Unknown58;

				struct TagBlock18
				{
					int Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock18, 0x4);

				struct TagBlock19
				{
					uint8_t Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock19, 0x1);

				struct TagBlock20
				{
					int Unknown0;
					int Unknown4;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock20, 0x8);

				struct TagBlock21
				{
					int Unknown0;
					int Unknown4;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock21, 0x8);

				struct TagBlock22
				{
					int16_t Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock22, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock17, 0x64);

			struct TagBlock23
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock23, 0x24);

			struct TagBlock24
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock24, 0x24);

			struct TagBlock25
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
				int Unknown78;
				int Unknown7C;
				int Unknown80;
				int Unknown84;
				int Unknown88;
				int Unknown8C;
				int Unknown90;
				int Unknown94;
				int Unknown98;
				int Unknown9C;
				int UnknownA0;
				int UnknownA4;
				int UnknownA8;
				int UnknownAC;
				int UnknownB0;
				int UnknownB4;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock25, 0xB8);

			struct TagBlock26
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock26, 0x30);

			struct TagBlock27
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock27, 0x78);

			struct TagBlock28
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock28, 0x30);

			struct TagBlock29
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
				int Unknown78;
				int Unknown7C;
				int Unknown80;
				int Unknown84;
				int Unknown88;
				int Unknown8C;
				int Unknown90;
				int Unknown94;
				int Unknown98;
				int Unknown9C;
				int UnknownA0;
				int UnknownA4;
				int UnknownA8;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock29, 0xAC);

			struct TagBlock30
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock30, 0x30);

			struct TagBlock31
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
				int Unknown78;
				int Unknown7C;
				int Unknown80;
				int Unknown84;
				int Unknown88;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock31, 0x8C);

			struct TagBlock32
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock32, 0x30);

			struct TagBlock33
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
				int Unknown78;
				int Unknown7C;
				int Unknown80;
				int Unknown84;
				int Unknown88;
				int Unknown8C;
				int Unknown90;
				int Unknown94;
				int Unknown98;
				int Unknown9C;
				int UnknownA0;
				int UnknownA4;
				int UnknownA8;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock33, 0xAC);

			struct TagBlock34
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock34, 0x30);

			struct TagBlock35
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
				int Unknown78;
				int Unknown7C;
				int Unknown80;
				int Unknown84;
				int Unknown88;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock35, 0x8C);

			struct TagBlock36
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock36, 0x30);

			struct TagBlock37
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock37, 0x70);

			struct TagBlock38
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock38, 0x30);

			struct TagBlock39
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock39, 0x54);

			struct TagBlock40
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock40, 0x30);

			struct TagBlock41
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
				int Unknown78;
				int Unknown7C;
				int Unknown80;
				int Unknown84;
				int Unknown88;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock41, 0x8C);

			struct TagBlock42
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock42, 0x30);

			struct TagBlock43
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock43, 0x30);

			struct TagBlock44
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock44, 0x30);

			struct TagBlock45
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock45, 0x30);

			struct TagBlock46
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock46, 0x30);

			struct TagBlock47
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock47, 0x30);

			struct TagBlock48
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock48, 0x30);

			struct TagBlock49
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock49, 0x30);

			struct TagBlock50
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock50, 0xC);

			struct PlayerStartingProfile
			{
				char Name[0x20];
				int Unknown20;
				int Unknown24;
				TagReference PrimaryWeapon;
				int16_t PrimaryRoundsLoaded;
				int16_t PrimaryRoundsTotal;
				TagReference SecondaryWeapon;
				int16_t SecondaryRoundsLoaded;
				int16_t SecondaryRoundsTotal;
				int Unknown50;
				int Unknown54;
				int8_t FragGrenades;
				int8_t PlasmaGrenades;
				int8_t SpikeGrenades;
				int8_t FirebombGrenades;
				int Unknown5C;
			};
			TAG_STRUCT_SIZE_ASSERT(PlayerStartingProfile, 0x60);

			struct TagBlock52
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock52, 0x1C);

			struct TagBlock53
			{
				struct TagBlock54;

				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				TagBlock<TagBlock54> Unknown44;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
				int Unknown78;

				struct TagBlock54
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
					int UnknownC;
					int Unknown10;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock54, 0x14);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock53, 0x7C);

			struct TagBlock55
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock55, 0x14);

			struct TagBlock56
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock56, 0x24);

			struct TagBlock57
			{
				TagReference Unknown0;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock57, 0x10);

			struct TagBlock58
			{
				struct TagBlock59;
				struct TagBlock60;

				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				TagBlock<TagBlock59> Unknown54;
				TagBlock<TagBlock60> Unknown60;

				struct TagBlock59
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
					int UnknownC;
					int Unknown10;
					int Unknown14;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock59, 0x18);

				struct TagBlock60
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock60, 0xC);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock58, 0x6C);

			struct TagBlock61
			{
				struct TagBlock62;

				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				TagBlock<TagBlock62> Unknown28;

				struct TagBlock62
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
					int UnknownC;
					int Unknown10;
					int Unknown14;
					int Unknown18;
					int Unknown1C;
					int Unknown20;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock62, 0x24);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock61, 0x34);

			struct TagBlock63
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock63, 0x28);

			struct TagBlock64
			{
				TagReference Unknown0;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock64, 0x10);

			struct TagBlock65
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
				int Unknown78;
				int Unknown7C;
				int Unknown80;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock65, 0x84);

			struct TagBlock66
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock66, 0x40);

			struct TagBlock67
			{
				struct TagBlock68;
				struct TagBlock69;

				int Unknown0;
				TagBlock<TagBlock68> Unknown4;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				TagBlock<TagBlock69> Unknown1C;

				struct TagBlock68
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
					int UnknownC;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock68, 0x10);

				struct TagBlock69
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
					int UnknownC;
					int Unknown10;
					int Unknown14;
					int Unknown18;
					int Unknown1C;
					int Unknown20;
					int Unknown24;
					int Unknown28;
					int Unknown2C;
					int Unknown30;
					int Unknown34;
					int Unknown38;
					int Unknown3C;
					int Unknown40;
					int Unknown44;
					int Unknown48;
					int Unknown4C;
					int Unknown50;
					int Unknown54;
					int Unknown58;
					int Unknown5C;
					int Unknown60;
					int Unknown64;
					int Unknown68;
					int Unknown6C;
					int Unknown70;
					int Unknown74;
					int Unknown78;
					int Unknown7C;
					int Unknown80;
					int Unknown84;
					int Unknown88;
					int Unknown8C;
					int Unknown90;
					int Unknown94;
					int Unknown98;
					int Unknown9C;
					int UnknownA0;
					int UnknownA4;
					int UnknownA8;
					int UnknownAC;
					int UnknownB0;
					int UnknownB4;
					int UnknownB8;
					int UnknownBC;
					int UnknownC0;
					int UnknownC4;
					int UnknownC8;
					int UnknownCC;
					int UnknownD0;
					int UnknownD4;
					int UnknownD8;
					int UnknownDC;
					int UnknownE0;
					int UnknownE4;
					int UnknownE8;
					int UnknownEC;
					int UnknownF0;
					int UnknownF4;
					int UnknownF8;
					int UnknownFC;
					int Unknown100;
					int Unknown104;
					int Unknown108;
					int Unknown10C;
					int Unknown110;
					int Unknown114;
					int Unknown118;
					int Unknown11C;
					int Unknown120;
					int Unknown124;
					int Unknown128;
					int Unknown12C;
					int Unknown130;
					int Unknown134;
					int Unknown138;
					int Unknown13C;
					int Unknown140;
					int Unknown144;
					int Unknown148;
					int Unknown14C;
					int Unknown150;
					int Unknown154;
					int Unknown158;
					int Unknown15C;
					int Unknown160;
					int Unknown164;
					int Unknown168;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock69, 0x16C);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock67, 0x28);

			struct TagBlock70
			{
				int16_t Unknown0;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock70, 0x2);

			struct TagBlock71
			{
				int Unknown0;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock71, 0x4);

			struct TagBlock72
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock72, 0x18);

			struct TagBlock73
			{
				int Unknown0;
				TagReference Unknown4;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				TagReference Unknown20;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock73, 0x58);

			struct TagBlock74
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock74, 0x78);

			struct TagBlock75
			{
				int Unknown0;
				int Unknown4;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock75, 0x8);

			struct TagBlock76
			{
				int Unknown0;
				TagReference Unknown4;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock76, 0x30);

			struct TagBlock77
			{
				struct TagBlock78;
				struct TagBlock79;
				struct TagBlock80;
				struct TagBlock81;
				struct TagBlock82;
				struct TagBlock83;
				struct TagBlock84;

				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				TagBlock<TagBlock78> Unknown10;
				TagBlock<TagBlock79> Unknown1C;
				TagBlock<TagBlock80> Unknown28;
				int Unknown34;
				TagBlock<TagBlock81> Unknown38;
				TagBlock<TagBlock82> Unknown44;
				TagBlock<TagBlock83> Unknown50;
				TagBlock<TagBlock84> Unknown5C;

				struct TagBlock78
				{
					int Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock78, 0x4);

				struct TagBlock79
				{
					int Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock79, 0x4);

				struct TagBlock80
				{
					int Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock80, 0x4);

				struct TagBlock81
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock81, 0xC);

				struct TagBlock82
				{
					int Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock82, 0x4);

				struct TagBlock83
				{
					int Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock83, 0x4);

				struct TagBlock84
				{
					int Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock84, 0x4);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock77, 0x68);

			struct TagBlock85
			{
				struct TagBlock86;
				struct TagBlock87;

				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				TagBlock<TagBlock86> Unknown54;
				TagBlock<TagBlock87> Unknown60;

				struct TagBlock86
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
					int UnknownC;
					int Unknown10;
					int Unknown14;
					int Unknown18;
					int Unknown1C;
					int Unknown20;
					int Unknown24;
					int Unknown28;
					int Unknown2C;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock86, 0x30);

				struct TagBlock87
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
					int UnknownC;
					int Unknown10;
					int Unknown14;
					int Unknown18;
					int Unknown1C;
					int Unknown20;
					int Unknown24;
					int Unknown28;
					int Unknown2C;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock87, 0x30);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock85, 0x6C);

			struct TagBlock88
			{
				struct TagBlock89;

				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				TagBlock<TagBlock89> Unknown24;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
				int Unknown78;
				int Unknown7C;
				int Unknown80;
				int Unknown84;
				int Unknown88;
				int Unknown8C;
				int Unknown90;
				int Unknown94;
				int Unknown98;
				int Unknown9C;
				int UnknownA0;
				int UnknownA4;
				int UnknownA8;
				int UnknownAC;
				int UnknownB0;

				struct TagBlock89
				{
					struct TagBlock90;
					struct TagBlock91;

					int Unknown0;
					TagBlock<TagBlock90> Unknown4;
					TagBlock<TagBlock91> Unknown10;

					struct TagBlock90
					{
						uint8_t Unknown0;
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock90, 0x1);

					struct TagBlock91
					{
						int16_t Unknown0;
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock91, 0x2);
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock89, 0x1C);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock88, 0xB4);

			struct TagBlock92
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock92, 0x30);

			struct TagBlock93
			{
				TagReference Unknown0;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock93, 0x10);

			struct TagBlock94
			{
				struct TagBlock95;
				struct TagBlock96;

				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				TagBlock<TagBlock95> Unknown10;
				TagBlock<TagBlock96> Unknown1C;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;

				struct TagBlock95
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
					int UnknownC;
					int Unknown10;
					int Unknown14;
					int Unknown18;
					int Unknown1C;
					int Unknown20;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock95, 0x24);

				struct TagBlock96
				{
					int Unknown0;
					int Unknown4;
					int Unknown8;
					int UnknownC;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock96, 0x10);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock94, 0x48);

			struct TagBlock97
			{
				TagReference Unknown0;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock97, 0x30);

			struct TagBlock98
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
				int Unknown20;
				int Unknown24;
				int Unknown28;
				int Unknown2C;
				int Unknown30;
				int Unknown34;
				int Unknown38;
				int Unknown3C;
				int Unknown40;
				int Unknown44;
				int Unknown48;
				int Unknown4C;
				int Unknown50;
				int Unknown54;
				int Unknown58;
				int Unknown5C;
				int Unknown60;
				int Unknown64;
				int Unknown68;
				int Unknown6C;
				int Unknown70;
				int Unknown74;
				int Unknown78;
				int Unknown7C;
				int Unknown80;
				int Unknown84;
				int Unknown88;
				int Unknown8C;
				int Unknown90;
				int Unknown94;
				int Unknown98;
				int Unknown9C;
				int UnknownA0;
				int UnknownA4;
				int UnknownA8;
				int UnknownAC;
				int UnknownB0;
				int UnknownB4;
				int UnknownB8;
				int UnknownBC;
				int UnknownC0;
				int UnknownC4;
				int UnknownC8;
				int UnknownCC;
				int UnknownD0;
				int UnknownD4;
				int UnknownD8;
				int UnknownDC;
				int UnknownE0;
				int UnknownE4;
				int UnknownE8;
				int UnknownEC;
				int UnknownF0;
				int UnknownF4;
				int UnknownF8;
				int UnknownFC;
				int Unknown100;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock98, 0x104);

			struct TagBlock99
			{
				int Unknown0;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock99, 0x4);

			struct TagBlock100
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock100, 0x18);

			struct TagBlock101
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock101, 0x10);

			struct TagBlock102
			{
				TagReference Unknown0;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock102, 0x10);
		};
		TAG_STRUCT_SIZE_ASSERT(Scenario, 0x824);
	}
}
