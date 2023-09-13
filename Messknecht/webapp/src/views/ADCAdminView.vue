<template>
  <BasePage :title="$t('adcadmin.ADCSettings')" :isLoading="dataLoading">
    <BootstrapAlert v-model="showAlert" dismissible :variant="alertType">
      {{ alertMessage }}
    </BootstrapAlert>

    <form @submit="saveADCConfig">
      <CardElement
        :text="$t('adcadmin.ADCChannelList')"
        textVariant="text-bg-primary"
        add-space
      >
        <InputElement
          :label="$t('adcadmin.ADCTypeName')"
          v-model="adConverter.adc.typename"
          type="text"
        />
        <InputElement
          :label="$t('adcadmin.ADCPollIntervall')"
          v-model="adConverter.adc.pollintervall"
          type="number"
          min="1"
          max="3600"
        />

        <div class="table-responsive">
          <table class="table">
            <thead>
              <tr>
                <th scope="col">{{ $t("adcadmin.ChannelNumber") }}</th>
                <th>{{ $t("adcadmin.Channelname") }}</th>
                <th>{{ $t("adcadmin.Unit") }}</th>
                <th>{{ $t("inverteradmin.Action") }}</th>
              </tr>
            </thead>
            <tbody ref="channelList">
              <tr
                v-for="channel in adConverter.adc.channels"
                v-bind:key="channel.id"
                :data-id="channel.id"
              >
                <td>{{ channel.id }}</td>
                <td>{{ channel.name }}</td>
                <td>{{ channel.unit }}</td>

                <td>                  
                  <a href="#" class="icon" :title="$t('adcadmin.EditChannel')">
                    <BIconPencil v-on:click="onOpenModal(modal, channel)" />
                  </a>
                </td>
              </tr>
            </tbody>
          </table>
        </div>
      </CardElement>
      <button type="submit" class="btn btn-primary mt-3 mb-3">
        {{ $t("adcadmin.Save") }}
      </button>
    </form>
  </BasePage>

  <div class="modal" id="adcEdit" tabindex="-1" role="dialog">
    <div class="modal-dialog modal-lg">
      <div class="modal-content">
        <div class="modal-header">
          <h5 class="modal-title">{{ $t("adcadmin.EditChannel") }}</h5>
          <button
            type="button"
            class="btn-close"
            data-bs-dismiss="modal"
            aria-label="Close"
          ></button>
        </div>
        <div class="modal-body">
          <form>
            <div class="mb-3">
              <CardElement
                :text="
                  $t('adcadmin.ChannelSettings') + ' ' + selectedChannel.id
                "
                addSpace
              >
                <!-- {{ datavalueList.datavalues }} -->
                <InputElement
                  :label="$t('adcadmin.Channelname')"
                  v-model="selectedChannel.name"
                  type="text"
                />
                <InputElement
                  :label="$t('adcadmin.Unit')"
                  v-model="selectedChannel.unit"
                  type="text"
                  :tooltip="$t('adcadmin.UnitHint')"
                />

                <InputElement
                  :label="$t('adcadmin.DMin')"
                  v-model="selectedChannel.dmin"
                  type="number"
                  min="0"
                />

                <InputElement
                  :label="$t('adcadmin.SMin')"
                  v-model="selectedChannel.smin"
                  type="number"
                  min="0"
                />

                <p>
                  <a
                    href="#"
                    role="button"
                    class="btn btn-secondary mb-3"
                    @click="onMeasureLow"
                    >{{ $t("adcadmin.Measure") }}</a
                  >
                </p>

                <InputElement
                  :label="$t('adcadmin.DMax')"
                  v-model="selectedChannel.dmax"
                  type="number"
                  min="0"
                  max="199999999999"
                />

                <InputElement
                  :label="$t('adcadmin.SMax')"
                  v-model="selectedChannel.smax"
                  type="number"
                  min="0"
                  max="199999999999"
                />

                <p>
                  <a
                    href="#"
                    role="button"
                    class="btn btn-secondary mb-3 popover-test"
                    title="Popover title"
                    data-content="Test"
                    @click="onMeasureHigh"
                    >{{ $t("adcadmin.Measure") }}</a
                  >
                </p>

                <InputElement
                  :label="$t('adcadmin.Takt')"
                  v-model="selectedChannel.takt"
                  type="number"
                  min="0"
                  max="3600"
                />
              </CardElement>
            </div>
          </form>
        </div>
        <div class="modal-footer">
          <button
            type="button"
            class="btn btn-secondary"
            @click="onCloseModal(modal)"
            data-bs-dismiss="modal"
          >
            {{ $t("adcadmin.Cancel") }}
          </button>
          <button type="button" class="btn btn-primary" @click="onEditSubmit">
            {{ $t("adcadmin.Take") }}
          </button>
        </div>
      </div>
    </div>
  </div>
  -
</template>

<script lang="ts">
import BasePage from "@/components/BasePage.vue";
import BootstrapAlert from "@/components/BootstrapAlert.vue";
import CardElement from "@/components/CardElement.vue";
import InputElement from "@/components/InputElement.vue";
import type { ADConfig, ADChannel } from "@/types/ADConfig";
import type { ADValue, ADValueList } from "@/types/ADValues";

import { authHeader, handleResponse } from "@/utils/authentication";
import { defineComponent } from "vue";
import * as bootstrap from "bootstrap";
import { BIconPencil } from "bootstrap-icons-vue";

export default defineComponent({
  components: {
    BIconPencil,
    BasePage,
    BootstrapAlert,
    CardElement,
    InputElement,
  },
  data() {
    return {
      modal: {} as bootstrap.Modal,
      dataLoading: true,
      adConverter: {} as ADConfig,
      selectedChannel: {} as ADChannel,
      datavalueList: {} as ADValueList,
      selectedValue: {} as ADValue,
      alertMessage: "",
      alertType: "info",
      showAlert: false,
    };
  },
  props: {
    wide: Boolean,
    tooltip: String,
  },
  
  mounted() {
    this.modal = new bootstrap.Modal("#adcEdit");
    
  },
  created() {
    this.getADConfig();
  },

  methods: {
    getADConfig() {
      this.dataLoading = true;
      fetch("/api/adc/config", { headers: authHeader() })
        .then((response) =>
          handleResponse(response, this.$emitter, this.$router)
        )
        .then((data) => {
          this.adConverter = data;
          this.dataLoading = false;
        });
    },
    saveADCConfig(e: Event) {
      e.preventDefault();
      const formData = new FormData();
      formData.append("data", JSON.stringify(this.adConverter));
      console.log("Post Data");
      console.log(JSON.stringify(this.adConverter));
      fetch("/api/adc/config", {
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
    onEditSubmit() {
      this.onCloseModal(this.modal);
    },
    onOpenModal(modal: bootstrap.Modal, channel: ADChannel) {
      console.log("Edit Channel");
      this.selectedChannel = channel;
      console.log(this.selectedChannel);
      modal.show();
    },
    onCloseModal(modal: bootstrap.Modal) {
      modal.hide();
    },
    onMeasureLow() {
      console.log("measure low");
      console.log(this.selectedChannel);
      this.dataLoading = true;
      fetch("/api/adc/read/raw", { headers: authHeader() })
        .then((response) =>
          handleResponse(response, this.$emitter, this.$router)
        )
        .then((data) => {
          this.datavalueList = data;
          console.log("measure low data");
          console.log(this.datavalueList);
          this.dataLoading = false;
          const channel = this.datavalueList.datavalues.find(
            (channel) => channel.id === this.selectedChannel.id
          ); 
          if (channel) this.selectedChannel.dmin = channel.value;
          
        });
    },
    onMeasureHigh() {
      console.log("measure high");
      console.log(this.selectedChannel);
      this.dataLoading = true;
      fetch("/api/adc/read/raw", { headers: authHeader() })
        .then((response) =>
          handleResponse(response, this.$emitter, this.$router)
        )
        .then((data) => {
          this.datavalueList = data;
          console.log("measure high data");
          console.log(this.datavalueList);
          this.dataLoading = false;
          console.log(this.selectedChannel.id);
          const channel = this.datavalueList.datavalues.find(
            (channel) => channel.id === this.selectedChannel.id
          );
          if (channel) this.selectedChannel.dmax = channel.value;
        });
    },
  },
});
</script>
