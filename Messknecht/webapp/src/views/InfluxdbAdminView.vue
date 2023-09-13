<template>
  <BasePage :title="$t('influxadmin.Setting')" :isLoading="dataLoading">
    <BootstrapAlert
      v-model="showAlert"
      dismissible
      :variant="alertType"
    >
      {{ alertMessage }}
    </BootstrapAlert>
     
    <form @submit="saveInfluxConfig">
      <CardElement
        :text="$t('influxadmin.Setting')"
        textVariant="text-bg-primary"
        add-space
      >
        <InputElement
          :label="$t('influxadmin.Url')"
          v-model="influxdbList.influxdb.url"
          type="text maxlength=64"
        />
        <InputElement
          :label="$t('influxadmin.Token')"
          v-model="influxdbList.influxdb.token"
          type="text maxlength=128"
        />
        <InputElement
          :label="$t('influxadmin.Org')"
          v-model="influxdbList.influxdb.org"
          type="text maxlength=64"
        />
        <InputElement
          :label="$t('influxadmin.Bucket')"
          v-model="influxdbList.influxdb.bucket"
          type="text maxlength=64"
        />
        <InputElement
          :label="$t('influxadmin.Name')"
          v-model="influxdbList.influxdb.name"
          type="text maxlength=64"
        />
      </CardElement>
      <button type="submit" class="btn btn-primary mt-3 mb-3">
        {{ $t("adcadmin.Save") }}
      </button>
    </form>
  </BasePage>
</template>

<script lang="ts">
import BasePage from "@/components/BasePage.vue";
import BootstrapAlert from "@/components/BootstrapAlert.vue";
import CardElement from "@/components/CardElement.vue";
import InputElement from "@/components/InputElement.vue";
import type { InfluxDbConfig } from "@/types/InfluxdbConfig";

import { authHeader, handleResponse } from "@/utils/authentication";
import { defineComponent } from "vue";
import * as bootstrap from "bootstrap";


export default defineComponent({
  components: {    
    BasePage,
    BootstrapAlert,
    CardElement,
    InputElement,
  },
  data() {
    return {
      modal: {} as bootstrap.Modal,
      dataLoading: true,
      influxdbList: {} as InfluxDbConfig,
      alertMessage: "",
      alertType: "info",
      showAlert: false,
    };
  },
  props: {
    wide: Boolean,
    tooltip: String,
  }, 
  created() {
    this.getInfluxConfig();
  },

  methods: {
    getInfluxConfig() {
      this.dataLoading = true;
      fetch("/api/influx/config", { headers: authHeader() })
        .then((response) =>
          handleResponse(response, this.$emitter, this.$router)
        )
        .then((data) => {
          this.influxdbList.influxdb = data;
          console.log(this.influxdbList);
          this.dataLoading = false;
        });
    },
    saveInfluxConfig(e: Event) {
      e.preventDefault();
            
      const formData = new FormData();
      formData.append("data", JSON.stringify(this.influxdbList));           
      console.log(JSON.stringify(this.influxdbList));
      fetch("/api/influx/config", {
        method: "POST",
        headers: authHeader(),
        body: formData,
      })
        .then((response) =>
          handleResponse(response, this.$emitter, this.$router)
        )
        .then((response) => {
          this.alertMessage = this.$t(
            "apiresponse." + response.code,
            response.param
          );
          this.alertType = response.type;
          this.showAlert = true;
        });
    },
  },
});
</script>
