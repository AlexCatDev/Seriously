#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

namespace BMFont {
	#define BMFONT_BLOCK_TYPE_INFO 1
	#define BMFONT_BLOCK_TYPE_COMMON 2
	#define BMFONT_BLOCK_TYPE_PAGES 3
	#define BMFONT_BLOCK_TYPE_CHARS 4
	#define BMFONT_BLOCK_TYPE_KERNINGS 5

	#define SIZEOF_CHAR 20
	#define SIZEOF_INFO 10
	#define SIZEOF_KERNINGPAIR 10
	#define SIZEOF_COMMON 15

	struct ByteStream {

	private:
		std::istream* stream;

	public:
		ByteStream(std::istream* stream)
		{
			this->stream = stream;
		}

		void Skip(size_t size) {
			this->stream->seekg(size, std::ios::_Seekdir::_Seekcur);
		}

		template<typename T>
		T Read() {
			T val;
			stream->read((char*)&val, sizeof(T));

			return val;
		}

		template<typename T>
		void Read(T* t) {
			stream->read((char*)t, sizeof(T));
		}

		template<typename T>
		void Read(T* t, std::streamsize size) {
			stream->read((char*)t, size);
		}

		template<typename T>
		T Read(std::streamsize size) {
			T val;
			stream->read((char*)&val, size);

			return val;
		}

		bool IsEOF() { return stream->eof(); }
	};


	struct Info {
		int16_t FontSize;
		uint8_t BitField;
		uint8_t CharSet;
		uint16_t StretchHorizontal;
		uint8_t SuperSamplingLevel;
		uint8_t PaddingUp, PaddingRight, PaddingDown, PaddingLeft;
		uint8_t SpacingHorizontal, SpacingVertical;
		uint8_t Outline;
		char* FontName;
	};

	struct Common {
		uint16_t LineHeight;
		uint16_t Base;
		uint16_t ScaleWidth;
		uint16_t ScaleHeight;
		uint16_t Pages;
		uint8_t BitField;
		uint8_t AlphaChannel;
		uint8_t RedChannel;
		uint8_t GreenChannel;
		uint8_t BlueChannel;
	};

	struct Char {
		uint32_t ID;
		uint16_t X, Y;
		uint16_t Width, Height;
		int16_t XOffset, YOffset;
		int16_t XAdvance;
		uint8_t Page;
		uint8_t Channel;
	};

	struct KerningPair {
		uint32_t First, Second;
		int16_t Amount;
	};

	struct Font {
	private:
		std::unordered_map<uint32_t, Char> chars;
		std::unordered_map<uint64_t, int16_t> kerningPairs;
		Common* common;
		Info* info;
		std::vector<std::string> filenames;
	public:
		Font(std::ifstream& stream)
		{
			parseFromStream(stream);
		}

		Font(const char* filename)
		{
			auto stream = std::ifstream(filename, std::ifstream::binary);
			parseFromStream(stream);
		}

		const Common* GetCommon()
		{
			return common;
		}

		const Info* GetInfo()
		{
			return info;
		}

		const Char* GetChar(uint32_t car) {
			return &chars.at(car);
		}

		int16_t GetKerningAmount(uint32_t firstChar, uint32_t secondChar) {
			return kerningPairs.at(firstChar);
		}

	private:
		void parseFromStream(std::ifstream& ifstream) {
			ByteStream stream(&ifstream);

			char* fileType = new char[4];
			fileType[3] = '/0';

			stream.Read(fileType, 3);

			uint8_t fileVersion;

			stream.Read(&fileVersion);

			info = new Info();
			common = new Common();


			while (ifstream.good())
			{
				uint8_t blockID = stream.Read<uint8_t>();

				int32_t blockSize = stream.Read<int32_t>();

				switch (blockID)
				{
				case BMFONT_BLOCK_TYPE_INFO:
				{
					stream.Read(info, SIZEOF_INFO);
					auto remainder = blockSize - SIZEOF_INFO;

					info->FontName = new char[remainder + 1];
					info->FontName[remainder + 1] = '\0';

					stream.Read(info->FontName, remainder);

					std::cout << "info: " << SIZEOF_INFO << "\n";

					break;
				}

				case BMFONT_BLOCK_TYPE_COMMON:
				{
					stream.Read(common, SIZEOF_COMMON);

					std::cout << "common: " << SIZEOF_COMMON << "\n";

					break;
				}

				case BMFONT_BLOCK_TYPE_PAGES:
				{
					char pageNameBuffer[64];
					pageNameBuffer[63] = '\0';
					for (size_t i = 0; i < common->Pages; i++)
					{
						/*
						char* pageName = new char[blockSize + 1];
						pageName[blockSize + 1] = '\0';

						stream.Read(pageName, blockSize);
						*/

						stream.Read(&pageNameBuffer, blockSize);

						filenames.emplace_back(pageNameBuffer);
					}

					std::cout << "pages: " << common->Pages << "\n";

					break;
				}
				case BMFONT_BLOCK_TYPE_CHARS:
				{
					auto charCount = blockSize / SIZEOF_CHAR;

					Char c;
					for (size_t i = 0; i < charCount; i++)
					{
						stream.Read(&c, SIZEOF_CHAR);
						//std::cout << "Parsing char ID: " << c.ID << " X: " << c.X << " Y: " << c.Y << "\n";
						chars[c.ID] = c;
					}

					std::cout << "chars: " << charCount << "\n";

					break;
				}
				case BMFONT_BLOCK_TYPE_KERNINGS:
				{
					auto kerningPairCount = blockSize / SIZEOF_KERNINGPAIR;

					KerningPair kPair;
					for (size_t i = 0; i < kerningPairCount; i++)
					{
						stream.Read(&kPair, SIZEOF_KERNINGPAIR);
						
						uint64_t key = *(uint64_t*)&kPair;

						kerningPairs[key] = kPair.Amount;
					}

					std::cout << "kerningpairs: " << kerningPairCount << "\n";

					break;
				}
				}
			}
		}
	};
}