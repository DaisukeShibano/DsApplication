#ifndef _DS_AS_NODE_
#define _DS_AS_NODE_

namespace DsLib
{
	class DsASNode
	{
	public:
		DsASNode();
		virtual ~DsASNode();

	public:
		virtual void OnActive(double dt) {}
		virtual void Update(double dt){}
		virtual void OnDeactive(double dt) {}

		virtual bool IsNext()const { return false; }
		virtual int GetNextNum() const { return 0; }
		virtual DsASNode** GetNext() { return NULL; }

	private:

	};


}

#endif
