tlock
=====

The simple virtual terminal locker.

By default, tlock auths $USER using pam. The authentication module used can be
configured in `/etc/pam.d/tlock`. For example, to enable ldap auth (assuming
ldap is properly configured), change `/etc/pam.d/tlock` to

    auth	sufficient	pam_unix.so nodelay
    auth	sufficient	pam_ldap.so

If pam is unavailable or undesired, set USE\_PAM in `Makefile.conf` to `false`
before compiling. In this case, tlock must be suid root to auth.
