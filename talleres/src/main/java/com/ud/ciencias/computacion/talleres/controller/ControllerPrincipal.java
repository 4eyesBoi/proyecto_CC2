package com.ud.ciencias.computacion.talleres.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;

import com.ud.ciencias.computacion.talleres.services.BusquedaLinealService;

@Controller
public class ControllerPrincipal {

    private final BusquedaLinealService busquedaLinealService;

    public ControllerPrincipal(BusquedaLinealService busquedaLinealService) {
        this.busquedaLinealService = busquedaLinealService;
    }

    @GetMapping("/")
    public String home() {
        return "index"; // templates/index.html
    }
    
    @GetMapping("/servicios/activos")
    public String serviciosActivos() {

        this.busquedaLinealService.buscarServicioActivo();

        System.out.println("Ingrese servicios activos");
        return "prueba"; // templates/prueba.html
    }
}
