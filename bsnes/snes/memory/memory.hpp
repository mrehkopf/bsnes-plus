struct Memory {
  virtual inline unsigned size() const;
  virtual uint8 read(unsigned addr) = 0;
  virtual void write(unsigned addr, uint8 data) = 0;
};

struct MMIO {
  virtual uint8 mmio_read(unsigned addr) = 0;
  virtual void mmio_write(unsigned addr, uint8 data) = 0;
};

struct UnmappedMemory : Memory {
  unsigned size() const;
  uint8 read(unsigned);
  void write(unsigned, uint8);
};

struct UnmappedMMIO : MMIO {
  uint8 mmio_read(unsigned);
  void mmio_write(unsigned, uint8);
};

struct StaticRAM : Memory {
  inline uint8* data();
  inline unsigned size() const;

  inline uint8 read(unsigned addr);
  inline void write(unsigned addr, uint8 n);
  inline uint8& operator[](unsigned addr);
  inline const uint8& operator[](unsigned addr) const;

  inline StaticRAM(unsigned size);
  inline ~StaticRAM();

private:
  uint8 *data_;
  unsigned size_;
};

struct MappedRAM : Memory {
  inline void reset();
  inline void map(uint8*, unsigned);
  inline void copy(const uint8*, unsigned);

  inline void write_protect(bool status);
  inline uint8* data();
  inline unsigned size() const;

  inline uint8 read(unsigned addr);
  inline void write(unsigned addr, uint8 n);
  inline const uint8& operator[](unsigned addr) const;
  inline MappedRAM();

private:
  uint8 *data_;
  unsigned size_;
  bool write_protect_;
};

struct MMIOAccess : Memory {
  MMIO* handle(unsigned addr);
  void map(unsigned addr, MMIO &access);
  uint8 read(unsigned addr);
  void write(unsigned addr, uint8 data);
  MMIOAccess();

private:
  MMIO *mmio[0x8000];
};

struct Bus {
  unsigned mirror(unsigned addr, unsigned size);
  void map(unsigned addr, Memory &access, unsigned offset);
  enum class MapMode : unsigned { Direct, Linear, Shadow };
  void map(MapMode mode,
    uint8  bank_lo, uint8  bank_hi,
    uint16 addr_lo, uint16 addr_hi,
    Memory &access, unsigned offset = 0, unsigned size = 0);

  alwaysinline uint8 read(uint24 addr);
  alwaysinline void write(uint24 addr, uint8 data);

  alwaysinline bool is_mirror(uint24 addr1, uint24 addr2);
  
  bool load_cart();
  void unload_cart();

  void power();
  void reset();

  struct Page {
    Memory *access;
    unsigned offset;
  } page[65536];

  void serialize(serializer&);

private:
  void map_reset();
  void map_xml();
  void map_system();
};

namespace memory {
  extern MMIOAccess mmio;   //S-CPU, S-PPU
  extern StaticRAM wram;    //S-CPU
  extern StaticRAM apuram;  //S-SMP, S-DSP
  extern StaticRAM vram;    //S-PPU
  extern StaticRAM oam;     //S-PPU
  extern StaticRAM cgram;   //S-PPU

  extern UnmappedMemory memory_unmapped;
  extern UnmappedMMIO mmio_unmapped;
};

extern Bus bus;
