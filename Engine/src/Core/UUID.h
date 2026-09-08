#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_set>

namespace Refraction {
	typedef uint64_t UUIDValue;

	class UUID {
	public:
		// Returns a UUID equivalent to null
		static UUID Null() { return {0}; }

		// Initialises a UUID using its int64 value and adds it to the generator history
		// Note: Returns a null UUID if the provided UUID is already in generator history, unless specified not to
		static UUID FromExisting(UUIDValue id, bool ignoreExisting = false);
		// Initialises a UUID from a UUID::Serialise() string and adds it to the generator history
		// Note: Returns a null UUID if the provided UUID is already in generator history
		static UUID Deserialise(std::string serialised);

		// Converts a UUID int to a formatted string
		static inline std::string AsString(UUIDValue value) {
			return UUID::FromExisting(value, true).AsString();
		}

		// The UUID generator uses the system clock and RNG along with a repetition counter to hopefully guarantee a universally unique ID
		// It is formatted as such:
		// Section 1: Seconds since epoch as 4 digit hex
		// Section 2: Milliseconds since epoch as 4 digit hex*
		// Section 3: Random number as 4 digit hex
		// Section 4: Random number as 4 digit hex
		// * The repetition counter is appended here
		// The repetition counter ticks up every time the generated UUID already exists in the internal generator history, and resets every time a new UUID is requested
		UUID();
		UUID(const UUID& other);
		~UUID() { Reset(); };

		// Returns a string with each section seperated by dashes
		[[nodiscard]] std::string AsString() const;
		// Returns a 64-bit integer of each 16-bit section concatenated
		[[nodiscard]] UUIDValue AsInt() const;
		// Zeroes all values (becomes a null UUID) and allows another to take the generated UUID
		void Reset();
		// Returns true if valid (not null)
		[[nodiscard]] bool IsValid() const { return (*this) != Null(); }
		// Returns the UUID serialised
		[[nodiscard]] std::string Serialise() const;

		bool operator==(const UUID& other) const { return AsInt() == other.AsInt(); }
		bool operator!=(const UUID& other) const { return AsInt() != other.AsInt(); }

		operator std::string() const { return AsString(); }
		operator UUIDValue() const { return AsInt(); }

	private:
		static std::unordered_set<UUIDValue> UUIDHistory;

		// Generates a UUID where all sections hold the same value. Not added to generator history.
		UUID(uint16_t initValue);

		uint16_t mElapsedSeconds = 0;
		uint16_t mElapsedMilliseconds = 0;
		uint16_t mRandomFirst = 0;
		uint16_t mRandomSecond = 0;
	};

	namespace Utilities {
		std::vector<UUIDValue> ToInts(std::vector<UUID> uuids);
		std::vector<UUID> FromInts(std::vector<UUIDValue> uuids);
		std::vector<std::string> ToStrings(std::vector<UUID> uuids);
	}
}
