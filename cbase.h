#ifndef __CBaseObject_Included__
#define __CBaseObject_Included__

class CEdict;

class CBaseObject
{
	public:
		CBaseObject() : edict(NULL) {};
		~CBaseObject();

		CEdict *edict;


		void RemoveOnBorder();

		virtual void Draw() {}
		virtual void Think() {}

		// Input controlled On's
		virtual void OnFire() {}
		virtual void OnMoveForward() {}
		virtual void OnMoveBackward() {}

		virtual void OnRemove() {}
		virtual bool OnBlock() { return false; }
};

#endif
