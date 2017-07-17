#pragma once
#include <string>
#include <fnd/types.h>
#include <fnd/memory_blob.h>
#include <fnd/List.h>
#include <crypto/aes.h>
#include <crypto/sha.h>
#include <nx/ISerialiseableBinary.h>

namespace nx
{
	class NcaHeader : public ISerialiseableBinary
	{
	public:
		struct sSection
		{
			u64 offset;
			u64 size;
			u8 key_type;
			crypto::sha::sSha256Hash hash;

			const sSection& operator=(const sSection& other)
			{
				offset = other.offset;
				size = other.size;
				key_type = other.key_type;
				hash = other.hash;

				return *this;
			}

			bool operator==(const sSection& other) const
			{
				return (offset == other.offset) \
					&& (size == other.size) \
					&& (key_type == other.key_type) \
					&& (hash == other.hash);
			}

			bool operator!=(const sSection& other) const
			{
				return operator==(other);
			}
		};

		static const size_t kDefaultBlockSize = 0x200;

		NcaHeader();
		NcaHeader(const NcaHeader& other);
		NcaHeader(const u8* bytes, size_t len);

		bool operator==(const NcaHeader& other) const;
		bool operator!=(const NcaHeader& other) const;
		void operator=(const NcaHeader& other);

		// to be used after export
		const u8* getBytes() const;
		size_t getSize() const;

		// export/import binary
		void exportBinary();
		void importBinary(const u8* bytes, size_t len);

		// variables
		void clear();
		u64 getNcaSize() const;
		void setNcaSize(u64 size);
		u64 getProgramId() const;
		void setProgramId(u64 program_id);
		u32 getUnk() const;
		const fnd::List<sSection>& getSections() const;
		void addSection(const sSection& section);
		const fnd::List<crypto::aes::sAes128Key>& getAesKeys() const;
		void addKey(const crypto::aes::sAes128Key& key);

	private:
		const std::string kModuleName = "NCA_HEADER";
		const std::string kNcaSig = "NCA2";
		static const size_t kSectionNum = 4;
		static const size_t kAesKeyNum = 4;

#pragma pack (push, 1)

		struct sNcaHeader
		{
		private:
			u8 signature_[4];
			u8 reserved_0[2];
			u16 block_size_;
			u64 nca_size_;
			u64 program_id_;
			u8 reserved_1[4];
			u32 unk_0_;
			u8 reserved_2[0x20];
			struct sNcaSection
			{
			private:
				u32 start_; // block units
				u32 end_; // block units
				u8 key_type_;
				u8 reserved[7];
			public:
				u32 start() const { return le_word(start_); }
				void set_start(u32 offset) { start_ = le_word(offset); }

				u32 end() const { return le_word(end_); }
				void set_end(u32 offset) { end_ = le_word(offset); }

				u8 key_type() const { return key_type_; }
				void set_key_type(u8 key_type) { key_type_ = key_type; }
			} section_[kSectionNum];
			crypto::sha::sSha256Hash section_hash_[kSectionNum];
			crypto::aes::sAes128Key aes_keys_[kAesKeyNum];
		public:
			const char* signature() const { return (const char*)signature_; }
			void set_signature(const char* signature) { memcpy(signature_, signature, 4); }

			u16 block_size() const { return le_hword(block_size_); }
			void set_block_size(u16 block_size) { block_size_ = le_hword(block_size); }

			u64 nca_size() const { return le_dword(nca_size_); }
			void set_nca_size(u64 nca_size) { nca_size_ = le_dword(nca_size); }

			u64 program_id() const { return le_dword(program_id_); }
			void set_program_id(u64 program_id) { program_id_ = le_dword(program_id); }

			u32 unk0() const { return le_word(unk_0_); }
			void set_unk0(u32 val) { unk_0_ = le_word(val); }

			const sNcaSection& section(u8 index) const { return section_[index%kSectionNum]; }
			sNcaSection& section(u8 index) { return section_[index%kSectionNum]; }

			const crypto::sha::sSha256Hash& section_hash(u8 index) const { return section_hash_[index%kSectionNum]; }
			crypto::sha::sSha256Hash& section_hash(u8 index) { return section_hash_[index%kSectionNum]; }

			const crypto::aes::sAes128Key& aes_key(u8 index) const { return aes_keys_[index%kAesKeyNum]; }
			crypto::aes::sAes128Key& aes_key(u8 index) { return aes_keys_[index%kAesKeyNum]; }
		};
#pragma pack (pop)

		// binary
		fnd::MemoryBlob mBinaryBlob;

		// data
		u16 mBlockSize;
		u64 mNcaSize;
		u64 mProgramId;
		u32 mUnk0;
		fnd::List<sSection> mSections;
		fnd::List<crypto::aes::sAes128Key> mAesKeys;

		u64 blockNumToSize(u32 block_num) const;
		u32 sizeToBlockNum(u64 real_size) const;
		bool isEqual(const NcaHeader& other) const;
		void copyFrom(const NcaHeader& other);
	};

}