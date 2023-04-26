using std::find;

template <class T>
bool NTFSUtils::hasElement(const vector<T> vec, T element) {
	return find(vec.begin(), vec.end(), element) != vec.end();
}

