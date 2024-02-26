# Backup daemon

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Preset

Make source/destination dirs if needed

Change `backup_daemon.ini` with your settings (from build dir):

```bash
sudo cp backup_daemon /usr/bin/
sudo cp ../backup_daemon.ini /etc/
sudo cp ../backup_daemon.service /etc/systemd/system/
```

## Usage

Update system and start

```bash
sudo systemctl daemon-reload
sudo systemctl start backup_daemon.service
```

Add to autorun

```bash
sudo systemctl enable backup_daemon.service
```

Stop

```bash
sudo systemctl stop backup_daemon.service
```

Restart

```bash
sudo systemctl restart backup_daemon.service
```

Show status/logs

```bash
sudo systemctl status backup_daemon.service
```
