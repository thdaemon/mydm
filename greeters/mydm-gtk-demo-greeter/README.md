## mydm-gtk-demo-greeter

mydm's gtk demo greeter

In top source directory

```
make gtk_greeter
sudo make gtk_greeter_install
```

In this directory

```
make
sudo make install
```

The greeter is no test currently on freebsd now.

You need GTK+3 to compile it.

Screenshot:

![Screenshot](https://raw.githubusercontent.com/thdaemon/mydm/master/greeters/mydm-gtk-demo-greeter/pic/screenshot.png)

### The Desktop Environment

Use the script file /usr/lib/mydm/mydm-gtk-demo-greeter/deinit to set the desktop environment, the default start xterm.

### Use MIT-MAGIC-COOKIE-1 XSecurity

you need run mydm like

```
XAUTHORITY=/run/mydm/tmpxxxx mydm -d :0 -v vt7 -c /usr/lib/mydm/mydm-gtk-demo-greeter/mydm-gtk-demo-greeter -g -- -auth /run/mydm/tmpxxxx
```

Use XAUTHORITY env to tell greeter which file will be use (the parent directory is need exist), and use `-auth` to tell X Server

And then, user need check the `Enforce MIT-MAGIC-COOKIE-1 XSecurity` checkbox in the greeter window, the XSecurity will work.

Fortunately, we provided a script to accomplish this tedious job, you just need

```
mydm-greeter-xsec-run -d :0 -v vt7
```

Use /etc/mydm/default-greeter to set the greeter path.
