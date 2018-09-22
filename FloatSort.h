#ifndef FLOATSORT
#define FLOATSORT

template<class D>
struct FloatSortElt
{
	float x;
	D     data;
	FloatSortElt(const float _x, const D & _data) { x = _x; data = _data; }
	FloatSortElt(const FloatSortElt<D> & elt) { x = elt.x; data = elt.data; };
};

template<class D>
class FloatSort
{
public:
	std::vector<FloatSortElt<D>> eltList;

	void sort()
	{
		std::sort(eltList.begin(), eltList.end(), [](FloatSortElt<D> & a, FloatSortElt<D> & b) {
			return a.x < b.x;
		});
	}
	void Sort(std::vector<float> & xList, std::vector<D> & dataList)
	{
		eltList.clear();
		auto iter2 = dataList.begin();
		for (auto iter = xList.begin(); iter != xList.end(); ++iter, ++iter2) {
			eltList.push_back(FloatSortElt<D>(*iter, *iter2));
		}
		sort();
	}
	void Sort(const float * xList, const D * dataList, const int dataCount)
	{
		eltList.clear();
		for (int i = 0; i < dataCount; ++i) {
			FloatSortElt<D> x(xList[i], dataList[i]);
			eltList.push_back(x);
		}
		sort();
	}
	FloatSortElt<D> & operator [] (const int id) { return eltList[id]; }
};

#endif