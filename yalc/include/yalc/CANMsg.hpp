/*!
 * @file 	CANOpenMsg.hpp
 * @brief	Type definitions
 * @author 	Christian Gehring
 * @date 	Jan, 2012
 * @version 1.0
 * @ingroup robotCAN
 *
 */

#ifndef CANMsg_HPP_
#define CANMsg_HPP_

#include <algorithm>
#include <stdint.h>
#include <initializer_list>

//! General CANOpen message container

class CANMsg {
public:
	/*! Constructor
	 * @param	COBId	Communication Object Identifier
	 */
	CANMsg():
		CANMsg(0)
	{

	}

	CANMsg(const uint32_t COBId):
		COBId_(COBId),
		flag_(false),
		length_{0},
		data_{0, 0, 0, 0, 0, 0, 0, 0}
	{

	}

	CANMsg(const uint32_t COBId, const uint8_t length, const uint8_t* data):
		COBId_(COBId),
		flag_(false),
		length_(length),
		data_{0, 0, 0, 0, 0, 0, 0, 0}
	{
		std::copy(&data[0], &data[length], data_);
	}

	CANMsg(const uint32_t COBId, const uint8_t length, const std::initializer_list<uint8_t> data):
		COBId_(COBId),
		flag_(false),
		length_(length),
		data_{0, 0, 0, 0, 0, 0, 0, 0}
	{
		std::copy(data.begin(), data.end(), data_);
	}

	//! Destructor
	virtual ~CANMsg() { }

	/*! Gets the Communication Object Identifier
	 *
	 * @return COBId
	 */
	inline uint32_t getCOBId() const { return COBId_; }

	/*! Gets flag whether the message will be sent or the message is received
	 * @return
	 */
	inline bool getFlag() const { return flag_; }

	/*! Gets the stack of values
	 *
	 * @return reference to data_[8]
	 */
	inline const uint8_t* getData() const { return data_; }

	/*! Gets the lengths of the values in the stack
	 * @return reference to length
	 */
	inline uint8_t getLength() const { return length_; }

	/*! Sets the flag if the message needs to be sent
	 * @param flag	if true message is sent
	 */
	inline void setFlag(const bool flag) { flag_ = flag; }

	/*! Sets the stack of values
	 * @param value	 array of length 8
	 */
	void setData(const uint8_t length, const uint8_t* data) {
		length_ = length;
		std::copy(&data[0], &data[length], data_);
	}

	inline void write(int32_t value, uint8_t pos)
	{
		data_[3 + pos] = static_cast<uint8_t>((value >> 24) & 0xFF);
		data_[2 + pos] = static_cast<uint8_t>((value >> 16) & 0xFF);
		data_[1 + pos] = static_cast<uint8_t>((value >> 8) & 0xFF);
		data_[0 + pos] = static_cast<uint8_t>((value >> 0) & 0xFF);
	}

	inline void write(uint32_t value, uint8_t pos)
	{
		data_[3 + pos] = static_cast<uint8_t>((value >> 24) & 0xFF);
		data_[2 + pos] = static_cast<uint8_t>((value >> 16) & 0xFF);
		data_[1 + pos] = static_cast<uint8_t>((value >> 8) & 0xFF);
		data_[0 + pos] = static_cast<uint8_t>((value >> 0) & 0xFF);
	}

	inline void write(int16_t value, uint8_t pos)
	{
		data_[1 + pos] = static_cast<uint8_t>((value >> 8) & 0xFF);
		data_[0 + pos] = static_cast<uint8_t>((value >> 0) & 0xFF);
	}

	inline void write(uint16_t value, uint8_t pos)
	{
		data_[1 + pos] = static_cast<uint8_t>((value >> 8) & 0xFF);
		data_[0 + pos] = static_cast<uint8_t>((value >> 0) & 0xFF);
	}

	inline void write(int8_t value, uint8_t pos)
	{
		data_[0 + pos] = static_cast<uint8_t>(value);
	}

	inline void write(uint8_t value, uint8_t pos)
	{
		data_[0 + pos] = value;
	}

	inline int32_t readint32(uint8_t pos) const
	{
		return (static_cast<int32_t>(data_[3 + pos]) << 24)
				| (static_cast<int32_t>(data_[2 + pos]) << 16)
				| (static_cast<int32_t>(data_[1 + pos]) << 8)
				| (static_cast<int32_t>(data_[0 + pos]));
	}

	inline uint32_t readuint32(uint8_t pos) const
	{
		return (static_cast<uint32_t>(data_[3 + pos]) << 24)
				| (static_cast<uint32_t>(data_[2 + pos]) << 16)
				| (static_cast<uint32_t>(data_[1 + pos]) << 8)
				| (static_cast<uint32_t>(data_[0 + pos]));
	}

	inline int16_t readint16(uint8_t pos) const
	{
		return (static_cast<int16_t>(data_[1 + pos]) << 8)
				| (static_cast<int16_t>(data_[0 + pos]));
	}

	inline uint16_t readuint16(uint8_t pos) const
	{
		return (static_cast<uint16_t>(data_[1 + pos]) << 8)
				| (static_cast<uint16_t>(data_[0 + pos]));
	}

	inline int8_t readint8(uint8_t pos) const
	{
		return static_cast<int8_t>(data_[0 + pos]);
	}

	inline uint8_t readuint8(uint8_t pos) const
	{
		return data_[0 + pos];
	}

private:
	//! Communication Object Identifier
	uint32_t COBId_;

	//! if true, the message will be sent or the message is received
	bool flag_;

	//! the message data length
	uint8_t length_;

	/*! Data of the CAN message
	 */
	uint8_t data_[8];
};

#endif /* CANMsg_HPP_ */