struct EntityTraits {
  static const int EntityCount = 65536;  // 16 bits
  static const int TypeCount = 256;      // 8 bits
  static const int GenerationCount = 16; // 4 bits
};

struct SmallEntityTraits {
  static const int EntityCount = 4;     // 2 bits
  static const int TypeCount = 4;       // 2 bits
  static const int GenerationCount = 4; // 2 bits
};