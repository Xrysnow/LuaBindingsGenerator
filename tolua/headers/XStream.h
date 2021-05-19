﻿#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include "base/CCData.h"
#include "base/CCRef.h"
#include "XBuffer.h"

namespace lstg
{
	class Stream : public cocos2d::Ref
	{
	public:

		enum SeekOrigin
		{
			/** Seek from the beginning. */
			BEG = 0,
			/** Seek from current position. */
			CUR = 1,
			/** Seek from the end. */
			END = 2
		};

		Stream() = default;
		virtual ~Stream() = default;

		/** If the stream is writable. */
		virtual bool isWritable() { return writable; }

		/** If the stream is resizable. */
		virtual bool isResizable() { return resizable; }

		/** Size of the stream. */
		virtual uint64_t size() { return _size; }

		/** Resize the stream. */
		virtual bool resize(uint64_t size) = 0;

		/** Get the position to read or write. */
		virtual uint64_t tell() { return pos; }

		/** Set the position to read or write. */
		virtual bool seek(SeekOrigin Origin, int64_t Offset) = 0;

		/**
		 * Read bytes from the stream.
		 * @param dst      target buffer
		 * @param length     length to read
		 * @param bytesRead length actually read, can be NULL
		 */
		virtual bool read(uint8_t* dst, uint64_t length, uint64_t* bytesRead = nullptr) = 0;

		/**
		 * Write bytes to the stream.
		 * @param src        source buffer
		 * @param length      length to write
		 * @param bytesWrite length actually write, can be NULL
		 */
		virtual bool write(const uint8_t* src, uint64_t length, uint64_t* bytesWrite = nullptr) = 0;

		/**
		 * Lock the stream. It is necessary when used in multi-thread.
		 *
		 * @note May cause block.
		 */
		virtual void lock();

		/**
		 * Try to lock the stream, will return false when already locked.
		 *
		 * This function will not block.
		 */
		virtual bool tryLock();

		/**
		 * Unlock the stream.
		 */
		virtual void unlock();

		/** Read from this and write to dst. */
		virtual bool fill(Stream* dst, uint64_t length, Buffer* buffer);

	protected:
		uint64_t _size = 0;
		uint64_t pos = 0;
		bool resizable = false;
		bool writable = false;
	private:
		std::mutex mut;
	public:
		Stream(const Stream &) = delete;
		Stream &operator =(const Stream &) = delete;
	};

	class StreamFile :
		public Stream
	{
	private:
		std::fstream file;
		std::string _path;
	public:
		bool resize(uint64_t length) override;
		bool seek(SeekOrigin origin, int64_t offset) override;
		bool read(uint8_t* dst, uint64_t length, uint64_t* bytesRead) override;
		bool write(const uint8_t* src, uint64_t length, uint64_t* bytesWrite) override;

		static StreamFile* create(const std::string& path, bool canWrite);
	protected:
		bool init(const std::string& path, bool canWrite);

		StreamFile();
		virtual ~StreamFile();
	};

	class StreamMemory :
		public Stream
	{
	private:
		Buffer* _buffer = nullptr;
	public:
		bool isWritable() override;
		bool isResizable() override;

		uint64_t size() override;
		bool resize(uint64_t length) override;
		uint64_t tell() override;
		bool seek(SeekOrigin origin, int64_t offset) override;
		bool read(uint8_t* dst, uint64_t length, uint64_t* bytesRead) override;
		bool write(const uint8_t* src, uint64_t length, uint64_t* bytesWrite) override;

		uint8_t* data();
		Buffer* getBuffer() const { return _buffer; }

		/** src can be NULL */
		static StreamMemory* create(Buffer* src = nullptr, bool copy = false);
	protected:
		bool init(Buffer* src, bool copy);

		StreamMemory();
		~StreamMemory();
	};

}
