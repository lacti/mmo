all:
	hg pull \
		&& hg update \
		&& xbuild Server.sln /p:Configuration=Release \
		&& (screen -S pylon -X quit || true) \
		&& screen -dmS pylon ./start_pylon.sh \
		&& (screen -S server -X quit || true) \
		&& screen -dmS server ./start_server.sh
