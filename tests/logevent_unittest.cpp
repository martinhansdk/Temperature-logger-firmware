#include "logevent.h"
#include "gtest/gtest.h" 	
#include <string> 

using namespace std;

namespace {

// The fixture for testing class Foo.
class LoggerTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  LoggerTest() {
    // You can do set-up work for each test here.
  }

  virtual ~LoggerTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};

string convBase(uint64_t v, long base)
{
	string digits = "0123456789abcdef";
	string result;
	if((base < 2) || (base > 16)) {
		result = "Error: base out of range.";
	}
	else {
		do {
			result = digits[v % base] + result;
			v /= base;
		}
		while(v);
	}
	return result;
}

::std::ostream& operator<<(::std::ostream& os, const LogEvent& event) {
  return os << "time=" << event.time 
	    << " t_indoor=" << event.temperature_indoor
            << " t_outdoor=" << event.temperature_outdoor 
	    << " door_open=" << (int)event.door_open;
  }

::std::ostream& operator<<(::std::ostream& os, const PackedLogEvent& event) {
  return os << hex << setiosflags (ios_base::showbase )
	    << (int)event.data[6] << " "
    	    << (int)event.data[5] << " "
	    << (int)event.data[4] << " "
	    << (int)event.data[3] << " "
	    << (int)event.data[2] << " "
	    << (int)event.data[1] << " "
	    << (int)event.data[0];
  }


// Tests that the packing and unpacking again yield the same result
TEST_F(LoggerTest, packing_and_unpacking_works) {
  LogEvent a;

  a.time = 0xffffffff;
  a.temperature_indoor = 0;
  a.temperature_outdoor = 0x3ff;
  a.door_open = 0;

  /*
  cout << "big_size=" << sizeof(uint64_t) << endl;
  cout << "time_mask=" << setw(64) << right << convBase(((uint64_t)PackedLogEvent::TIME_MASK) << PackedLogEvent::TIME_OFFSET, 2) << endl;
  cout << "tin_mask= "  << setw(64) << right << convBase(((uint64_t)PackedLogEvent::TEMPERATURE_INDOOR_MASK) << PackedLogEvent::TEMPERATURE_INDOOR_OFFSET, 2) << endl;
  cout << "tout_mask=" << setw(64) << right << convBase(((uint64_t)PackedLogEvent::TEMPERATURE_OUTDOOR_MASK) << PackedLogEvent::TEMPERATURE_OUTDOOR_OFFSET, 2) << endl;
  cout << "door_mask=" << setw(64) << right << convBase(((uint64_t)PackedLogEvent::DOOR_OPEN_MASK) << PackedLogEvent::DOOR_OPEN_OFFSET, 2) << endl;

  cout << "b=" << ::testing::PrintToString(b) << endl;
  */
  EXPECT_EQ(a, a.pack().unpack());

  a.time = 0;
  a.temperature_indoor = 0x3ff;
  a.temperature_outdoor = 0;
  a.door_open = 1;

  EXPECT_EQ(a, a.pack().unpack());

  // random testing
  for(int i=0; i<10000; i++) {
    a.time = rand();
    a.temperature_indoor = rand() % (1 << 10);
    a.temperature_outdoor = rand() % (1 << 10);
    a.door_open = rand() % 2;
    EXPECT_EQ(a, a.pack().unpack());
  }
}


}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
