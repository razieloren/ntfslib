import datetime
import struct
import time
import sys


class DirProduct(object):

    # 12:00AM, 1/1/1601
    _FILETIME_ZERO_DATE = datetime.datetime(1601, 1, 1, 12, 0, 0)
    _DATE_FORMAT = "%x - %X"

    def __init__(self, file_name, flags, file_size, file_real_size, creation_time, data_change_time, mft_change_time, access_time, record_number, parent_record_number):
        self._file_name = file_name.decode('utf-16')
        self._flags = self._parse_bitmask(flags, 8)
        self._file_size = file_real_size
        self._data_chane_time = data_change_time
        self._record = record_number
        self._parent_record = parent_record_number
        self._children = []
        
        # Extended data, might be needed, for now, "disabled" (Saves sizeof(long) * 4 bytes per record in your memory).
        # self._creation_time = creation_time
        # self._mft_change_time = mft_change_time
        # self._access_time = access_time

    def add_child(self, child):
        self._children.append(child)

    def dump_children_hierarchy(self, output_file, depth):
        flags = ("D" if self.is_directory else "")   + \
                ("X" if self.is_deleted else "")     + \
                ("R" if self.is_read_only else "")   + \
                ("H" if self.is_hidden else "")      + \
                ("S" if self.is_system_file else "") + \
                ("C" if self.is_compressed else "") + \
                ("E" if self.is_encrypted else "") + \
                ("A" if self.is_archived else "")
        data_change_time = self._filetime_to_datetime(self._data_chane_time).strftime(self._DATE_FORMAT)
        size_in_mb = self._file_size / 1024.0 / 1024.0

        output_file.write("{0}{1} <{2}> ({3}MB) Modified: {4}\n".format("\t" * depth, self._file_name, flags, size_in_mb, data_change_time))
        for child in self._children:
            child.dump_children_hierarchy(output_file, depth + 1)

    @staticmethod
    def _parse_bitmask(value, num_of_bits):
        return [bool(value & (1 << num_of_bits - i - 1)) for i in range(num_of_bits)]

    def _filetime_to_datetime(self, filetime):
        return self._FILETIME_ZERO_DATE + datetime.timedelta(microseconds=filetime/10)

    @property
    def is_directory(self):
        return self._flags[7]

    @property
    def is_deleted(self):
        return self._flags[6]

    @property
    def is_read_only(self):
        return self._flags[5]

    @property
    def is_hidden(self):
        return self._flags[4]

    @property
    def is_system_file(self):
        return self._flags[3]
        
    @property
    def is_compressed(self):
        return self._flags[2]

    @property
    def is_encrypted(self):
        return self._flags[1]
        
    @property
    def is_archived(self):
        return self._flags[0]

    @property
    def file_name(self):
        return self._file_name

    @property
    def parent_record(self):
        return self._parent_record

    @property
    def record(self):
        return self._record
    

class DirParser(object):

    def __init__(self):
        self._root = None
        self._products = []

    def parse(self, product_path):
        print("Parsing product: {0}".format(product_path))
        print("Loading all records from file...")
        start = time.time()
        with open(product_path, "rb") as product_file:
            name_size_str = product_file.read(2)
            while name_size_str != b'':
                name_length = struct.unpack("<H", name_size_str)[0]
                temp_product = DirProduct(*struct.unpack("<{name_length}sBQQQQQQQQ".format(name_length=name_length), product_file.read(name_length + 65)))          
                self._products.append(temp_product)
                name_size_str = product_file.read(2)
        print("All records loaded ({0} seconds).".format(time.time() - start))

        print("Sorting records...")
        start = time.time()
        for product in self._products:
            if product.record == 5:
                self._root = product
            else:
                father = self._find_father(product)
                if father:
                    father.add_child(product)
            # Only NTFS meta files don't have a parent directory, we don't want to show them anyways.
            # If you are intereted in these files, that's where you should catch them.
        print("Sorting completed ({0} seconds).".format(time.time() - start))


    def _find_father(self, product):
        first = 0;
        last = len(self._products) - 1

        while first <= last:
            midpoint = (first + last) // 2
            temp_father = self._products[midpoint]
            if temp_father.record == product.parent_record:
                return temp_father
            else:
                if product.parent_record < temp_father.record:
                    last = midpoint - 1
                else:
                    first = midpoint + 1

        return None

    def dump_dir(self, output_file_path):
        with open(output_file_path, "w", encoding="utf8") as output_file:
            print("Dumping full records hierarchy...")
            start = time.time()
            self._root.dump_children_hierarchy(output_file, 0)
            print("Done dumping full records hierarchy ({0} seconds).".format(time.time() - start))
    

def main(product_path, full_dir_output_file):
    start = time.time()
    parser = DirParser()
    parser.parse(product_path)
    parser.dump_dir(full_dir_output_file)
    print("Finished in {0} seconds.".format(time.time() - start))

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: {0} <NTFSLib Full Dir Product File Path> <Full Dir Output File>".format(sys.argv[0]))
    else:
        main(sys.argv[1], sys.argv[2])