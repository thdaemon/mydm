## Register mydm as a system service

Making mydm a system service is often useful. Before this feature is developed, if a user want to let mydm boot automatically start, he had to mydm complete start command written in such a file like `/etc/rc.local`

Now, the good news comes, mydm can be installed as a system service. The mydm suite now comes with a script called `mydm-service-install` that allows users to automate installation services on their own systems.

> mydm-service-install now only used in a LSB-supported initialization system. Development of other initialization systems is under way.

```
mydm-service-install --target linux-lsb --default
```

install for LSB target, all options are filled in by default

```
mydm-service-install --target linux-lsb
```

install for LSB target, the script queries each installation option in an interactive manner, leaving it blank by default.

```
mydm-service-install --target linux-lsb --uninstall
```

uninstall for LSB target.

