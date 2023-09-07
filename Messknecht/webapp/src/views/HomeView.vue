<template>
  <BasePage
    :title="$t('home.LiveData')"
    :isLoading="dataLoading"
    :isWideScreen="true"
  >
    <!-- <HintView :hints="liveData.hints" /> -->

    <CardElement
      :text="$t('adcadmin.ADCChannelList')"
      textVariant="text-bg-primary"
      add-space
    >
      <div class="table-responsive">
        <table class="table">
          <thead>
            <tr>
              <th scope="col">{{ $t("adcadmin.ChannelNumber") }}</th>
              <th>{{ $t("adcadmin.Channelname") }}</th>
              <th>{{ $t("adcadmin.ValueScaled") }}</th>
              <th>{{ $t("adcadmin.Unit") }}</th>
              <th>{{ $t("adcadmin.Takt") }}[s]</th>
            </tr>
          </thead>
          <tbody ref="channelList">
            <tr
              v-for="channel in liveData.datavalues"
              v-bind:key="channel.id"
              :data-id="channel.id"
            >
              <td>{{ channel.id }}</td>
              <td>{{ channel.name }}</td>

              <td>{{ channel.value.toFixed(3)}}</td>
              <td>{{ channel.unit }}</td>
              <td>{{ channel.takt }}</td>

              
            </tr>
          </tbody>
        </table>
      </div>
    </CardElement>
  </BasePage>
</template>

<script lang="ts">
import BasePage from "@/components/BasePage.vue";

import CardElement from "@/components/CardElement.vue";

import type { LiveADValueList } from "@/types/LiveDataStatus";
import {
  authHeader,
  authUrl,
  handleResponse,
  isLoggedIn,
} from "@/utils/authentication";

import { defineComponent } from "vue";

export default defineComponent({
  components: {
    BasePage,
    CardElement,
  },
  data() {
    return {
      isLogged: this.isLoggedIn(),
      socket: {} as WebSocket,      
      liveData: {} as LiveADValueList,      
      heartInterval: 0,
      dataAgeInterval: 0,
      dataLoading: true,
      isFirstFetchAfterConnect: true,
    };
  },
  created() {
    this.dataLoading = true;
    this.getADConfig();
    this.getInitialData();
    this.initSocket();   

    this.$emitter.on("logged-in", () => {
      this.isLogged = this.isLoggedIn();
    });
    this.$emitter.on("logged-out", () => {
      this.isLogged = this.isLoggedIn();
    });
  },
  
  unmounted() {
    this.closeSocket();
  }, 

  methods: {
    isLoggedIn,
    getADConfig() {
      /* this.dataLoading = true;
      fetch("/api/adc/config", { headers: authHeader() })
        .then((response) =>
          handleResponse(response, this.$emitter, this.$router)
        )
        .then((data) => {
          console.log(data);
          this.adConverter = data;
          this.dataLoading = false;
        }); */
    },
    getInitialData() {
      this.dataLoading = true;
      fetch("/api/livedata/status", { headers: authHeader() })
        .then((response) =>
          handleResponse(response, this.$emitter, this.$router)
        )
        .then((data) => {
          this.liveData = data;
          console.log("liveData-Status");
          console.log(this.liveData);
          this.dataLoading = false;

          /* const channel = this.datavalueList.datavalues.find(
            (channel) => channel.id === this.selectedChannel.id
          );
          if (channel) this.selectedChannel.dmin = channel.value; */
        });
    },
    initSocket() {
      console.log("Starting connection to WebSocket Server");

      const { protocol, host } = location;
      const authString = authUrl();
      console.log("test2");
      console.log(authString);
     

      const webSocketUrl = `${
        protocol === "https:" ? "wss" : "ws"
      }://${authString}${host}/livedata`;
      
      this.socket = new WebSocket(webSocketUrl);
      
      
      this.socket.onmessage = (event) => {        
        //console.log(event);        
        this.liveData = JSON.parse(event.data);
        console.log("test");
        console.log(this.liveData);
        this.dataLoading = false;
        //this.heartCheck(); // Reset heartbeat detection
      };

      this.socket.onopen = function (event) {
        console.log(event);
        console.log("Successfully connected to the echo websocket server...");        
      };

      // Listen to window events , When the window closes , Take the initiative to disconnect websocket Connect
      window.onbeforeunload = () => {
        this.closeSocket();
      };
    },
    initDataAgeing() {
      /* this.dataAgeInterval = setInterval(() => {
                if (this.inverterData) {
                    this.inverterData.forEach(element => {
                        element.data_age++;
                    });
                }
            }, 1000); */
    },
    // Send heartbeat packets regularly * 59s Send a heartbeat
    heartCheck() {
      /* this.heartInterval && clearTimeout(this.heartInterval);
            this.heartInterval = setInterval(() => {
                if (this.socket.readyState === 1) {
                    // Connection status
                    this.socket.send("ping");
                } else {
                    this.initSocket(); // Breakpoint reconnection 5 Time
                }
            }, 59 * 1000); */
    },
    /** To break off websocket Connect */
    closeSocket() {
      this.socket.close();
      // this.heartInterval && clearTimeout(this.heartInterval);
      this.isFirstFetchAfterConnect = true;
    },
  },
});
</script>
