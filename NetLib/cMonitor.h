#pragma once

class NETLIB_API cMonitor
{
public :
      class NETLIB_API Owner
      {
         public:
            explicit Owner(
               cMonitor &crit);

            ~Owner();
         private :
		    cMonitor &m_csSyncObject;
            // No copies do not implement
		    Owner(const Owner &rhs);
		    Owner &operator=(const Owner &rhs);
      };

      cMonitor();
      ~cMonitor();

#if(_WIN32_WINNT >= 0x0400)
      BOOL TryEnter();
#endif
      void Enter();
      void Leave();

   private :

	  CRITICAL_SECTION m_csSyncObject;

      cMonitor(const cMonitor &rhs);
      cMonitor &operator=(const cMonitor &rhs);
};
