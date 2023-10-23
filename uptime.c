#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct _stodi
{
	long long boot_time;
	long long current_time;
	char pad[12];
} system_time_info;

#if defined(__x86_64__)
typedef long long
#else
typedef int
#endif
(__stdcall *zw_query_system_information)(
	int type,
	void *buffer,
	unsigned long buffer_size,
	unsigned long *unused);

#if defined(__x86_64__)
typedef long long
#else
typedef int
#endif
(__cdecl *zw_sprintf)(
	char *buffer,
	const char *format,
	...);

int mainCRTStartup (void)
{
	char *p;
	int plen;
	HANDLE handle;
	char buf[1024];
	char *d = "days";
	char *h = "hours";
	char *m = "minutes";
	long long uptime = 0;
	int updays, uphours, upmins;
	system_time_info stodi;
	zw_query_system_information query_info;
	zw_sprintf format_string;

	handle = GetStdHandle (STD_OUTPUT_HANDLE);
	query_info = (zw_query_system_information)GetProcAddress (GetModuleHandle (TEXT("ntdll.dll")), "ZwQuerySystemInformation");
	format_string = (zw_sprintf)GetProcAddress (GetModuleHandle (TEXT("ntdll.dll")), "sprintf"); /* antivirus doesn't like this */
	if(query_info != NULL && format_string != NULL)
	{
		/* NT_SYSTEM_TIME_OF_DAY_INFORMATION = 3 */
		query_info (3, &stodi, sizeof(stodi), NULL);
		uptime = (stodi.current_time - stodi.boot_time) / 10000000ULL;

		if(uptime <= 0)
			ExitProcess (1);

		p = GetCommandLineA ();
		plen = lstrlen(p);
		if(plen)
			if(p[plen - 1] == 's' && (p[plen - 2] == '-' || p[plen - 2] == '/'))
			{
				format_string (buf, "%d\n", uptime);
				WriteConsoleA (handle, buf, lstrlen(buf), NULL, NULL);
				ExitProcess (0);
			}

		updays = uptime / 86400;
		uphours = (uptime - (updays * 86400)) / 3600;
		upmins = (uptime - (updays * 86400) - (uphours * 3600)) / 60;

		if(updays == 1)
			d = "day";
		if(uphours == 1)
			h = "hour";
		if(upmins == 1)
			m = "minute";

		if(updays && uphours && upmins)
			format_string (buf, "%d %s, %d %s, %d %s\n", updays, d, uphours, h, upmins, m);
		else if(updays && uphours)
			format_string (buf, "%d %s, %d %s\n", updays, d, uphours, h);
		else if(updays && upmins)
			format_string (buf, "%d %s, %d %s\n", updays, d, upmins, m);
		else if(updays)
			format_string (buf, "%d %s\n", updays, d);
		else if(uphours && upmins)
			format_string (buf, "%d %s, %d %s\n", uphours, h, upmins, m);
		else if(uphours)
			format_string (buf, "%d %s\n", uphours, h);
		else
			format_string (buf, "%d %s\n", upmins, m);
		WriteConsoleA (handle, buf, lstrlen(buf), NULL, NULL);
		ExitProcess (0);
	}
	ExitProcess (1);
}
