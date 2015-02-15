/*
 * Noncopyable.h
 *
 */

#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

namespace shared
{

class Noncopyable
{
protected:
	Noncopyable() {}
	~Noncopyable() {}
private:
	Noncopyable(const Noncopyable&);
	const Noncopyable& operator=(const Noncopyable&);
};

} //namespace shared
#endif /* NONCOPYABLE_H_ */
